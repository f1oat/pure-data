use core::slice;
use std::collections::HashMap;
use std::ffi::{CStr, CString};
use std::net::IpAddr;
use std::os::raw::c_char;
use std::os::raw::c_void;
use std::str::FromStr;
use std::time::{Duration, Instant};

use mdns_sd::{Error, Receiver, ServiceDaemon, ServiceEvent, ServiceInfo, UnregisterStatus};

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct mdns_txt_prop {
    key: *const c_char,
    value: *const c_char,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct mdns_service_info {
    /// service type, for ex.: _osc._udp.local.
    stype: *const c_char,
    fullname: *const c_char, // <instance>.<service>.<domain>
    hostname: *const c_char, // fully qualified name for service host
    /// service port
    port: u16,
    host_ttl: u32,  // used for SRV and Address records
    other_ttl: u32, // used for PTR and TXT records
    priority: u16,
    weight: u16,
    /// pointer to array of ip addresses
    ip: *const *const c_char,
    /// number of service ip addresses
    ip_len: usize,
    /// pointer to array of txt properties
    txt: *const mdns_txt_prop,
    /// number of txt properties
    txt_len: usize,
}

/// `ty_domain` is the service type and the domain label, for example
/// "_my-service._udp.local.".
///
/// `my_name` is the instance name, without the service type suffix.
///
/// `host_name` is the "host" in the context of DNS. It is used as the "name"
/// in the address records (i.e. TYPE_A and TYPE_AAAA records). It means that
/// for the same hostname in the same local network, the service resolves in
/// the same addresses. Be sure to check it if you see unexpected addresses resolved.
#[allow(non_camel_case_types)]
#[repr(C)]
pub struct mdns_service_info_register {
    /// service type, for example "_my-service._udp", or "_my-service._udp.local.".
    service: *const c_char,
    /// the instance name, without the service type suffix, for example "My Service v1.1"
    name: *const c_char,
    /// is the "host" in the context of DNS. It is used as the "name"
    /// in the address records (i.e. TYPE_A and TYPE_AAAA records). It means that
    /// for the same hostname in the same local network, the service resolves in
    /// the same addresses. Be sure to check it if you see unexpected addresses resolved.
    host: *const c_char,
    /// service port
    port: u16,
    /// pointer to array of ip addresses
    ip: *const *const c_char,
    /// number of service ip addresses
    ip_len: usize,
    /// pointer to array of txt properties
    txt: *const mdns_txt_prop,
    /// number of txt properties
    txt_len: usize,
}

impl mdns_txt_prop {
    fn new(key: &CStr, value: &CStr) -> Self {
        mdns_txt_prop {
            key: key.as_ptr(),
            value: value.as_ptr(),
        }
    }
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct mdns_cb_err {
    user: *mut c_void,
    cb: Option<extern "C" fn(user: *mut c_void, msg: *const c_char)>,
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct mdns_cb_srv {
    user: *mut c_void,
    cb: Option<
        extern "C" fn(user: *mut c_void, ty: *const c_char, fullname: *const c_char, found: bool),
    >,
}

impl mdns_cb_err {
    fn exec(&self, msg: &str) {
        let msg = CString::new(msg).unwrap_or_default();
        // println!("{msg}");
        self.cb.map(|f| f(self.user, msg.as_ptr()));
    }
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct mdns_cb_resolv {
    user: *mut c_void,
    cb: Option<extern "C" fn(user: *mut c_void, info: &mdns_service_info)>,
}

#[allow(non_camel_case_types)]
pub struct mdns {
    daemon: Option<ServiceDaemon>,
    on_err: mdns_cb_err,
    on_srv: mdns_cb_srv,
    on_resolv: mdns_cb_resolv,
    services: Vec<(String, Receiver<ServiceEvent>)>,
    register_list: HashMap<String, ServiceInfo>,
    // cache: HashMap<String, ServiceInfo>,
}

impl mdns {
    fn new(on_err: mdns_cb_err, on_srv: mdns_cb_srv, on_resolv: mdns_cb_resolv) -> Self {
        mdns {
            daemon: match ServiceDaemon::new() {
                Ok(d) => Some(d),
                Err(err) => {
                    on_err.exec(format!("creation error: {err}").as_str());
                    None
                }
            },
            on_err,
            on_srv,
            on_resolv,
            services: vec![],
            register_list: HashMap::new(),
        }
    }

    fn add_register(&mut self, name: &String, srv: ServiceInfo) -> bool {
        println!("service is registered: {name}");
        self.register_list.insert(name.clone(), srv).is_some()
    }

    fn del_register(&mut self, name: &String) -> bool {
        if self.register_list.remove(name).is_some() {
            println!("service is unregistered: {name}");
            true
        } else {
            println!("service is not found: {name}");
            false
        }
    }

    fn unregister_all(&mut self) {
        for (name, _) in self.register_list.iter() {
            mdns_do_unregister(self, name, 10, true);
            println!("service is unregistered: {name}");
        }

        self.register_list.clear();
    }

    fn is_ok(&self) -> bool {
        return self.daemon.is_some();
    }

    fn as_ref(&self) -> &ServiceDaemon {
        return self.daemon.as_ref().unwrap();
    }

    fn err(&self, msg: &str) {
        self.on_err.exec(msg)
    }

    fn add_service(&mut self, name: &String, s: Receiver<ServiceEvent>) {
        if self.services.iter().any(|(n, _)| *n == *name) {
            return;
        }
        self.services.push((name.clone(), s))
    }

    fn remove_service(&mut self, name: &String) {
        self.services.retain(|kv| kv.0 != *name);
    }

    fn _service(&self, ty: String, name: String, found: bool) {
        let ty = CString::new(ty.as_str()).unwrap_or_default();
        let name = CString::new(name.as_str()).unwrap_or_default();
        self.on_srv.cb.map(|f| {
            f(self.on_srv.user, ty.as_ptr(), name.as_ptr(), found);
        });
    }

    fn service_added(&self, ty: String, name: String) {
        self._service(ty, name, true)
    }

    fn service_removed(&self, ty: String, name: String) {
        self._service(ty, name, false)
    }

    fn service_resolved(&self, info: ServiceInfo) {
        self.on_resolv.cb.map(|f| {
            let hostname = CString::new(info.get_hostname()).unwrap_or_default();
            let fullname = CString::new(info.get_fullname()).unwrap_or_default();
            let stype = CString::new(info.get_type()).unwrap_or_default();
            let ip_str = &Vec::from_iter(
                info.get_addresses()
                    .into_iter()
                    .map(|x| CString::new(x.to_string()).unwrap_or_default()),
            );
            let ip_ptr = &Vec::from_iter(ip_str.iter().map(|str| str.as_ptr()));
            let props_vec = Vec::from_iter(info.get_properties().iter().map(|p| {
                (
                    CString::new(p.key()).unwrap_or_default(),
                    CString::new(p.val_str()).unwrap_or_default(),
                )
            }));

            let props_ptr = Vec::from_iter(props_vec.iter().map(|(k, v)| mdns_txt_prop::new(k, v)));

            let info = mdns_service_info {
                stype: stype.as_ptr(),
                fullname: fullname.as_ptr(),
                hostname: hostname.as_ptr(),
                port: info.get_port(),
                host_ttl: info.get_host_ttl(),
                other_ttl: info.get_other_ttl(),
                priority: info.get_priority(),
                weight: info.get_weight(),
                ip: ip_ptr.as_ptr(),
                ip_len: ip_ptr.len(),
                txt: props_ptr.as_ptr(),
                txt_len: props_ptr.len(),
            };

            f(self.on_resolv.user, &info);
        });
    }

    fn process_events(&mut self, deadline: Instant) {
        for (_name, rcv) in self.services.iter() {
            while let Ok(event) = rcv.recv_deadline(deadline) {
                match event {
                    ServiceEvent::ServiceFound(srv_type, fullname) => {
                        self.service_added(srv_type, fullname)
                    }
                    ServiceEvent::ServiceResolved(info) => {
                        self.service_resolved(info);
                    }
                    ServiceEvent::ServiceRemoved(srv_type, fullname) => {
                        self.service_removed(srv_type, fullname)
                    }
                    // ServiceEvent::SearchStarted(info) => todo!(),
                    // ServiceEvent::SearchStopped(_) => todo!(),
                    _ev => {
                        //println!("event: {_ev:?}")
                    }
                }
            }
        }
    }
}

#[derive(PartialEq, Debug)]
#[allow(non_camel_case_types)]
#[repr(C)]
pub enum mdns_rc {
    Ok,
    /// when NULL service pointer given
    NullService,
    ServiceError,
    InvalidString,
    BrowseFailed,
    SetOptionError,
    ServiceNotFound,
}

#[no_mangle]
/// create new MDNS service handler
/// @param on_err - error callback
/// @param on_srv - on service found/remove
/// @return pointer to MDNS service or NULL on error
pub extern "C" fn ceammc_mdns_create(
    on_err: mdns_cb_err,
    on_srv: mdns_cb_srv,
    on_resolv: mdns_cb_resolv,
) -> *mut mdns {
    let srv = mdns::new(on_err, on_srv, on_resolv);
    if !srv.is_ok() {
        std::ptr::null_mut()
    } else {
        Box::into_raw(Box::new(srv))
    }
}

#[no_mangle]
/// enable/disable network interfaces to search for mdns services
/// @param mdns - pointer to mdns struct
/// @param name - network interface name, for example:
///  - '*', 'all' (all network interfaces)
///  - 'ipv4' (all ip v4 interfaces)
///  - 'ipv6' (all ip v6 interfaces)
///  - '127.0.0.1' etc (interface ip address)
///  - 'lo0', 'eth1' (interface name)
///
/// @note '!' can't be added to disable interface, for ex.: '!ipv6'
/// @return mdns_rc::Ok on success or other codes on error
pub extern "C" fn ceammc_mdns_enable_iface(mdns: *mut mdns, name: *const c_char) -> mdns_rc {
    if mdns.is_null() {
        return mdns_rc::NullService;
    }

    let mdns = unsafe { &*mdns };
    if !mdns.is_ok() {
        mdns.err("service error");
        return mdns_rc::ServiceError;
    }

    let ifname = util::to_str(name);
    if let Err(rc) = ifname {
        mdns.err("invalid interface name");
        return rc;
    }
    let ifname = ifname.unwrap();

    let srv = mdns.as_ref();

    if ifname.starts_with("!") {
        let ifn = util::name_to_iface(&ifname[1..]);
        match srv.disable_interface(ifn) {
            Ok(_) => mdns_rc::Ok,
            _ => mdns_rc::SetOptionError,
        }
    } else {
        let ifn = util::name_to_iface(ifname.as_str());
        match srv.enable_interface(ifn) {
            Ok(_) => mdns_rc::Ok,
            _ => mdns_rc::SetOptionError,
        }
    }
}

#[no_mangle]
/// free mdns handler
/// @param mdns - pointer to mdns struct created with ceammc_mdns_create()
pub extern "C" fn ceammc_mdns_free(mdns: *mut mdns) {
    if !mdns.is_null() {
        let mut mdns = unsafe { Box::from_raw(mdns) };
        mdns.unregister_all();
        drop(mdns)
    }
}

fn do_browse(mdns: &mut mdns, sty: String, first_time: bool) -> mdns_rc {
    match mdns.as_ref().browse(sty.as_str()) {
        Ok(rcv) => {
            mdns.add_service(&sty, rcv);
            mdns_rc::Ok
        }
        Err(err) => {
            if first_time {
                match err {
                    Error::Again => {
                        // first time sleep and try again
                        std::thread::sleep(Duration::from_millis(10));
                        // recursive call
                        do_browse(mdns, sty, false)
                    }
                    _ => {
                        mdns.err(format!("subscribe error: {err}").as_str());
                        mdns_rc::ServiceError
                    }
                } // second time error
            } else {
                mdns_rc::ServiceError
            }
        }
    }
}

#[no_mangle]
/// subscribe to mdns service events
/// @param mdns - pointer to mdns
/// @param service: mdns service name
/// @return mdns_rc
/// @note can block up to 10ms in case of eagain
pub extern "C" fn ceammc_mdns_subscribe(mdns: *mut mdns, service: *const c_char) -> mdns_rc {
    if mdns.is_null() {
        return mdns_rc::NullService;
    }

    let mdns: &mut mdns = unsafe { &mut *mdns };
    if !mdns.is_ok() {
        mdns.err("service error");
        return mdns_rc::ServiceError;
    }

    let service = util::local_service_name(service);
    match service {
        Ok(service) => do_browse(mdns, service, true),
        Err(rc) => {
            mdns.err("invalid service name");
            return rc;
        }
    }
}

fn do_stop_browse(mdns: &mut mdns, sty: String, first_time: bool) -> mdns_rc {
    match mdns.as_ref().stop_browse(sty.as_str()) {
        Ok(_) => {
            mdns.remove_service(&sty);
            mdns_rc::Ok
        }
        Err(err) => {
            if first_time {
                match err {
                    Error::Again => {
                        // first time sleep and try again
                        std::thread::sleep(Duration::from_millis(10));
                        // recursive call
                        do_stop_browse(mdns, sty, false)
                    }
                    _ => {
                        mdns.remove_service(&sty);
                        mdns.err(format!("unsubscribe error: {err}").as_str());
                        mdns_rc::ServiceError
                    }
                }
            } else {
                // second time error
                mdns.remove_service(&sty);
                mdns_rc::ServiceError
            }
        }
    }
}

#[no_mangle]
/// unsubscribe from mdns service
/// @param mdns - pointer to mdns
/// @param service name
/// @return mdns_rc
pub extern "C" fn ceammc_mdns_unsubscribe(mdns: *mut mdns, service: *const c_char) -> mdns_rc {
    if mdns.is_null() {
        return mdns_rc::NullService;
    }

    let mdns = unsafe { &mut *mdns };
    if !mdns.is_ok() {
        mdns.err("service error");
        return mdns_rc::ServiceError;
    }

    let service = util::local_service_name(service);
    match service {
        Ok(service) => do_stop_browse(mdns, service, true),
        Err(rc) => {
            mdns.err("invalid service name");
            return rc;
        }
    }
}

#[no_mangle]
/// unsubscribe from mdns service
/// @param mdns - pointer to mdns
/// @return mdns_rc
pub extern "C" fn ceammc_mdns_process_events(mdns: *mut mdns, timeout_ms: u64) -> mdns_rc {
    if mdns.is_null() {
        return mdns_rc::NullService;
    }

    let mdns = unsafe { &mut *mdns };
    if !mdns.is_ok() {
        mdns.err("service error");
        return mdns_rc::ServiceError;
    }

    let now = std::time::Instant::now();
    let deadline = now.checked_add(Duration::from_millis(timeout_ms)).unwrap();
    mdns.process_events(deadline);
    mdns_rc::Ok
}

/// register mdns service
/// @param mdns - mdns service pointer
/// @param info - service info pointer
/// @return mdns_rc
#[no_mangle]
pub extern "C" fn ceammc_mdns_register(
    mdns: *mut mdns,
    info: *const mdns_service_info_register,
) -> mdns_rc {
    if mdns.is_null() {
        return mdns_rc::NullService;
    }

    let mdns = unsafe { &mut *mdns };
    if !mdns.is_ok() {
        return mdns_rc::ServiceError;
    }

    if info.is_null() {
        mdns.err("invalid service info pointer");
        return mdns_rc::ServiceError;
    }
    let info = unsafe { &*info };

    let service = util::local_service_name(info.service);
    let name: Result<String, mdns_rc> = util::to_str(info.name);
    let host = util::to_str(info.host);

    let strs = [&service, &name, &host];
    if strs.iter().any(|x| x.is_err()) {
        mdns.err("invalid string");
        return mdns_rc::InvalidString;
    }
    let [service, name, host] = strs.map(|x| x.as_ref().unwrap());

    // full service name: EXAMPLE._http._tcp.local
    let fullname = format!("{name}.{service}");

    let ips: String = if info.ip_len == 0 || info.ip.is_null() {
        String::default()
    } else {
        let mut ips = String::new();
        for ip in unsafe { slice::from_raw_parts(info.ip, info.ip_len) } {
            let ip = unsafe { CStr::from_ptr(*ip) }.to_str().unwrap_or_default();
            ips.push_str(ip);
            ips.push(',');
        }
        if ips.ends_with(',') {
            ips.pop();
        }

        ips
    };

    let props = if info.txt.is_null() || info.txt_len == 0 {
        None
    } else {
        let mut txt: HashMap<String, String> = HashMap::new();
        for p in unsafe { slice::from_raw_parts(info.txt, info.txt_len) } {
            let k = util::to_str(p.key);
            let v = util::to_str(p.value);
            if [&k, &v].iter().all(|s| s.is_ok()) {
                txt.insert(k.unwrap(), v.unwrap());
            }
        }
        Some(txt)
    };

    let srv = mdns.as_ref();

    return match ServiceInfo::new(
        service.as_str(),
        name.as_str(),
        util::add_local_domain_suffix(host).as_str(),
        ips,
        info.port,
        props,
    ) {
        Ok(info) => {
            let info = info.enable_addr_auto();
            println!("service: {info:?}");
            match srv.register(info.clone()) {
                Ok(_) => {
                    mdns.add_register(&fullname, info);
                    mdns_rc::Ok
                }
                Err(err) => {
                    mdns.err(&err.to_string());
                    mdns_rc::ServiceError
                }
            }
        }
        Err(err) => {
            mdns.err(&err.to_string());
            mdns_rc::ServiceError
        }
    };
}

#[no_mangle]
/// unregister MDNS service
/// @note can block timeout_ms on eagain socket error
/// @param mdns - mdns handle
/// @param name - instance name
/// @param service - mdns service type
/// @param timeout_ms - timeout for unregister in milliseconds
/// @return mdns_rc::Ok on success and other codes or error
pub extern "C" fn ceammc_mdns_unregister(
    mdns: *mut mdns,
    name: *const c_char,
    service: *const c_char,
    timeout_ms: u64,
) -> mdns_rc {
    if mdns.is_null() {
        return mdns_rc::NullService;
    }

    let mdns = unsafe { &mut *mdns };
    if !mdns.is_ok() {
        return mdns_rc::ServiceError;
    }

    match util::make_fullname(name, service) {
        Ok(fullname) => {
            let rc = mdns_do_unregister(mdns, fullname.as_str(), timeout_ms, true);
            if rc == mdns_rc::Ok {
                mdns.del_register(&fullname.to_owned());
            }
            rc
        }
        Err(err) => {
            mdns.err(format!("{err:?}").as_str());
            err
        }
    }
}

fn mdns_do_unregister(mdns: &mdns, fullname: &str, timeout_ms: u64, again: bool) -> mdns_rc {
    let srv = mdns.as_ref();
    let res = srv.unregister(fullname);
    if res.is_err() {
        if !again {
            return mdns_rc::ServiceError;
        }

        return match res.unwrap_err() {
            Error::Again => mdns_do_unregister(mdns, fullname, timeout_ms, again),
            x => {
                mdns.err(format!("unregister error: {x}").as_str());
                mdns_rc::ServiceError
            }
        };
    }

    let rc = res.unwrap().recv_timeout(Duration::from_millis(timeout_ms));

    match rc {
        Ok(st) => match st {
            UnregisterStatus::OK => mdns_rc::Ok,
            UnregisterStatus::NotFound => {
                mdns.err(format!("service not found: {fullname}").as_str());
                mdns_rc::ServiceNotFound
            }
        },
        Err(x) => {
            mdns.err(x.to_string().as_str());
            mdns_rc::ServiceError
        }
    }
}

mod util {
    use super::*;

    pub fn name_to_iface(name: &str) -> mdns_sd::IfKind {
        let ifn = match name {
            "all" => mdns_sd::IfKind::All,
            "*" => mdns_sd::IfKind::All,
            "ipv4" => mdns_sd::IfKind::IPv4,
            "ipv6" => mdns_sd::IfKind::IPv6,
            _ => match IpAddr::from_str(name) {
                Ok(addr) => mdns_sd::IfKind::Addr(addr),
                _ => mdns_sd::IfKind::Name(name.to_string()),
            },
        };
        ifn
    }

    pub fn to_str(msg: *const c_char) -> Result<String, mdns_rc> {
        if msg.is_null() {
            Err(mdns_rc::InvalidString)
        } else {
            match unsafe { CStr::from_ptr(msg) }.to_str() {
                Ok(str) => match String::from_str(str) {
                    Ok(str) => Ok(str),
                    Err(_err) => Err(mdns_rc::InvalidString),
                },
                Err(_err) => Err(mdns_rc::InvalidString),
            }
        }
    }

    pub fn add_local_domain_suffix(hostname: &String) -> String {
        if hostname.ends_with(".local.") {
            hostname.into()
        } else if hostname.ends_with(".local") {
            String::from(hostname) + "."
        } else if hostname.ends_with(".") {
            String::from(hostname) + "local."
        } else {
            String::from(hostname) + ".local."
        }
    }

    pub fn local_service_name(name: *const c_char) -> Result<String, mdns_rc> {
        match unsafe { CStr::from_ptr(name) }.to_str() {
            Ok(str) => Ok(add_local_domain_suffix(&str.to_owned())),
            Err(_) => Err(mdns_rc::InvalidString),
        }
    }

    /// make full service name
    /// @example EXAMPLE + _http._tcp. => EXAMPLE._http._tcp.local.
    pub fn make_fullname(name: *const c_char, service: *const c_char) -> Result<String, mdns_rc> {
        let host = to_str(name)?;
        let service = &local_service_name(service)?;
        Ok(format!("{host}.{service}"))
    }
}
