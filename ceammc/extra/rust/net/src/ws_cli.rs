pub mod ws_cli {
    use std::{
        ffi::{CStr, CString},
        net::TcpStream,
        os::raw::{c_char, c_void},
    };
    use tungstenite::{connect, stream::MaybeTlsStream, Error, Message, WebSocket};
    use url::Url;

    #[derive(Debug, PartialEq)]
    #[allow(non_camel_case_types)]
    #[repr(C)]
    pub enum ws_rc {
        Ok = 0,
        InvalidClient,
        InvalidServer,
        InvalidMessage,
        InvalidData,
        SendError,
        NoData,
        CloseError,
        ConnectionClosed,
        NonBlockingError,
        SocketAcceptError,
        SocketReadError,
        SocketDeferClose,
        RunloopExit,
    }

    #[allow(non_camel_case_types)]
    pub struct ws_client {
        ws: WebSocket<MaybeTlsStream<TcpStream>>,
        on_err: ws_callback_text,
        on_text: ws_callback_text,
        on_bin: ws_callback_data,
        on_ping: ws_callback_data,
        on_pong: ws_callback_data,
        on_close: ws_callback_data,
    }

    impl ws_client {
        fn err(&self, rc: ws_rc, msg: &str) -> ws_rc {
            self.on_err.cb.map(|f| {
                let msg = CString::new(msg);
                msg.map(|str| {
                    f(self.on_err.user, str.as_ptr());
                })
            });
            rc
        }

        fn text(&self, msg: &str) {
            self.on_text.cb.map(|f| {
                let msg = CString::new(msg);
                msg.map(|str| {
                    f(self.on_text.user, str.as_ptr());
                })
            });
        }

        fn bin(&self, data: &Vec<u8>) {
            self.on_bin.exec(data);
        }

        fn ping(&self, data: &Vec<u8>) {
            self.on_ping.exec(data);
        }

        fn pong(&self, data: &Vec<u8>) {
            self.on_pong.exec(data);
        }

        fn close(&self) -> ws_rc {
            self.on_close.exec(&vec![]);
            ws_rc::ConnectionClosed
        }
    }

    #[derive(Debug)]
    #[allow(non_camel_case_types)]
    #[repr(C)]
    pub struct ws_callback_text {
        pub user: *mut c_void,
        pub cb: Option<extern "C" fn(user: *mut c_void, msg: *const c_char)>,
    }

    #[derive(Debug)]
    #[allow(non_camel_case_types)]
    #[repr(C)]
    pub struct ws_callback_data {
        user: *mut c_void,
        cb: Option<extern "C" fn(user: *mut c_void, data: *const u8, len: usize)>,
    }

    impl ws_callback_data {
        pub fn exec(&self, data: &Vec<u8>) {
            self.cb.map(|f| {
                f(self.user, data.as_ptr(), data.len());
            });
        }
    }

    impl ws_callback_text {
        pub fn exec(&self, msg: &str) {
            self.cb.map(|f| {
                let msg = CString::new(msg);
                msg.map(|str| {
                    f(self.user, str.as_ptr());
                })
            });
        }
    }

    /// create websocket client
    /// @param url - string in format: ws://HOST:PORT?/path?
    /// @param on_err - callback for error messages
    /// @param on_text - callback for incoming text messages
    /// @param on_bin - callback for incoming binary messages
    /// @param on_ping - callback for incoming ping messages
    /// @param on_pong - callback for incoming pong messages
    /// @param on_close - callback on connection close
    /// @return pointer to client or NULL
    #[no_mangle]
    pub extern "C" fn ceammc_ws_client_create(
        url: *const c_char,
        on_err: ws_callback_text,
        on_text: ws_callback_text,
        on_bin: ws_callback_data,
        on_ping: ws_callback_data,
        on_pong: ws_callback_data,
        on_close: ws_callback_data,
    ) -> *mut ws_client {
        if url.is_null() {
            on_err.exec("null url pointer");
            return std::ptr::null_mut();
        }

        let url = unsafe { CStr::from_ptr(url) }.to_str();

        match url {
            Ok(url) => match Url::parse(url) {
                Ok(url) => match connect(url) {
                    Ok((ws, _resp)) => {
                        match ws.get_ref() {
                            MaybeTlsStream::Plain(sock) => {
                                // println!("response: {:?}", _resp);
                                let _ = sock.set_nonblocking(true).map_err(|err| {
                                    on_err.exec(
                                        format!("can't set socket to non-blocking: {err}").as_str(),
                                    )
                                });
                            }
                            _ => {}
                        }

                        return Box::into_raw(Box::new(ws_client {
                            ws,
                            on_err,
                            on_bin,
                            on_ping,
                            on_pong,
                            on_text,
                            on_close,
                        }));
                    }
                    Err(err) => {
                        on_err.exec(format!("websocket connection error: {err}").as_str());
                        std::ptr::null_mut()
                    }
                },
                Err(err) => {
                    on_err.exec(format!("connection error: {err}").as_str());
                    std::ptr::null_mut()
                }
            },
            Err(err) => {
                on_err.exec(format!("invalid URL: {err}").as_str());
                std::ptr::null_mut()
            }
        }
    }

    /// free websocket client
    /// @param cli - pointer to websocket client
    #[no_mangle]
    pub extern "C" fn ceammc_ws_client_free(cli: *mut ws_client) {
        if !cli.is_null() {
            let _ = unsafe { Box::from_raw(cli) };
        }
    }

    fn send_message(cli: &mut ws_client, msg: Message, flush: bool) -> ws_rc {
        // can't read message after close call
        if !cli.ws.can_write() {
            return cli.err(ws_rc::CloseError, "connection closed");
        }

        if flush {
            match cli.ws.send(msg) {
                Err(err) => cli.err(ws_rc::SendError, format!("send error: {err}").as_str()),
                _ => ws_rc::Ok,
            }
        } else {
            match cli.ws.write(msg) {
                Err(err) => cli.err(ws_rc::SendError, format!("write error: {err}").as_str()),
                _ => ws_rc::Ok,
            }
        }
    }

    #[no_mangle]
    /// sends text message to WebSocket server
    /// @param cli - pointer to ws client
    /// @param msg - text message
    /// @param flush - if true ensures all messages
    ///        previously passed to write and automatic queued pong responses are written & flushed into the underlying stream.
    /// @return ws_rc::Ok, ws_rc::InvalidClient, ws_rc::InvalidMessage, ws_rc::CloseError, ws_rc::SendError,
    pub extern "C" fn ceammc_ws_client_send_text(
        cli: *mut ws_client,
        msg: *const c_char,
        flush: bool,
    ) -> ws_rc {
        if cli.is_null() {
            return ws_rc::InvalidClient;
        }
        let cli = unsafe { &mut *cli };

        if msg.is_null() {
            return cli.err(ws_rc::InvalidMessage, "null message pointer");
        }
        let msg = unsafe { CStr::from_ptr(msg) }.to_str();
        match msg {
            Ok(str) => send_message(cli, Message::Text(str.to_string()), flush),
            Err(_) => cli.err(ws_rc::InvalidMessage, "invalid message"),
        }
    }

    #[no_mangle]
    /// sends binary message to WebSocket server
    /// @param cli - pointer to ws client
    /// @param data - data pointer
    /// @param len - data length
    /// @param flush - if true ensures all messages
    ///        previously passed to write and automatic queued pong responses are written & flushed into the underlying stream.
    /// @return ws_rc::Ok, ws_rc::InvalidClient, ws_rc::InvalidMessage, ws_rc::CloseError, ws_rc::SendError,
    pub extern "C" fn ceammc_ws_client_send_binary(
        cli: *mut ws_client,
        data: *const u8,
        len: usize,
        flush: bool,
    ) -> ws_rc {
        if cli.is_null() {
            return ws_rc::InvalidClient;
        }
        let cli = unsafe { &mut *cli };

        if data.is_null() || len == 0 {
            return cli.err(ws_rc::InvalidData, "invalid data");
        }

        let data = unsafe { std::slice::from_raw_parts(data, len) }.to_vec();

        send_message(cli, Message::Binary(data), flush)
    }

    /// sends ping to WebSocket server
    /// @param cli - pointer to websocket client
    /// @param data - pointer to ping data (can be NULL)
    /// @param len - data length
    /// @return ws_rc::Ok, ws_rc::InvalidClient, ws_rc::InvalidMessage, ws_rc::CloseError, ws_rc::SendError,
    #[no_mangle]
    pub extern "C" fn ceammc_ws_client_send_ping(
        cli: *mut ws_client,
        data: *const u8,
        len: usize,
    ) -> ws_rc {
        if cli.is_null() {
            return ws_rc::InvalidClient;
        }
        let cli = unsafe { &mut *cli };

        let data = if data.is_null() || len == 0 {
            vec![]
        } else {
            unsafe { std::slice::from_raw_parts(data, len) }.to_vec()
        };

        send_message(cli, Message::Ping(data), true)
    }

    #[allow(non_camel_case_types)]
    #[repr(C)]
    pub enum ws_trim {
        NO,    // no trim
        START, // remove leading whitespaces, newlines etc.
        END,   // remove trailing whitespaces, newlines etc.
        BOTH,  // remove leading and trailing whitespaces, newlines etc.
    }

    /// read and process all available messages from WebSocket server
    /// @param cli - pointer to websocket client
    /// @param trim - text message trim mode
    /// @return ws_rc::Ok, ws_rc::InvalidClient, ws_rc::InvalidMessage, ws_rc::CloseError, ws_rc::SendError,
    #[no_mangle]
    pub extern "C" fn ceammc_ws_client_read(cli: *mut ws_client, trim: ws_trim) -> ws_rc {
        if cli.is_null() {
            return ws_rc::InvalidClient;
        }
        let cli = unsafe { &mut *cli };

        // can't read message after close call
        if !cli.ws.can_read() {
            return ws_rc::CloseError;
        }

        loop {
            match cli.ws.read() {
                Ok(msg) => match msg {
                    Message::Text(txt) => {
                        let txt = match trim {
                            ws_trim::NO => txt.as_str(),
                            ws_trim::START => txt.trim_start(),
                            ws_trim::END => txt.trim_end(),
                            ws_trim::BOTH => txt.trim(),
                        };

                        println!("message: {}", txt);
                        cli.text(txt);
                    }
                    Message::Pong(data) => {
                        println!("pong: {:?}", data);
                        cli.pong(&data);
                    }
                    Message::Ping(data) => {
                        println!("ping: {:?}", data);
                        cli.ping(&data);
                        // pong reply
                        let _ = cli.ws.send(Message::Pong(data));
                    }
                    Message::Binary(data) => {
                        println!("binary: {:?}", data);
                        cli.bin(&data);
                    }
                    _ => (),
                },
                Err(err) => {
                    return match err {
                        Error::ConnectionClosed => cli.close(),
                        Error::AlreadyClosed => cli.err(ws_rc::NoData, "already closed"),
                        Error::Io(e) => {
                            // handle non-blocking read
                            match e.kind() {
                                std::io::ErrorKind::WouldBlock => ws_rc::RunloopExit,
                                _ => cli.err(ws_rc::NoData, format!("IO error: {e}").as_str()),
                            }
                        }
                        Error::Tls(e) => cli.err(ws_rc::NoData, format!("TLS error: {e}").as_str()),
                        Error::Capacity(e) => {
                            cli.err(ws_rc::NoData, format!("capacity error: {e}").as_str())
                        }
                        Error::Protocol(e) => {
                            cli.err(ws_rc::NoData, format!("protocol error: {e}").as_str())
                        }
                        Error::WriteBufferFull(e) => {
                            cli.err(ws_rc::NoData, format!("buffer error: {e}").as_str())
                        }
                        Error::Utf8 => cli.err(ws_rc::NoData, "Utf8 error"),
                        Error::AttackAttempt => cli.err(ws_rc::NoData, "attack attempt error"),
                        Error::Url(e) => cli.err(ws_rc::NoData, format!("url error: {e}").as_str()),
                        Error::HttpFormat(e) => {
                            cli.err(ws_rc::NoData, format!("http format error: {e}").as_str())
                        }
                        _ => cli.err(ws_rc::NoData, format!("error: {err}").as_str()),
                    };
                }
            }
        }
    }

    /// close client connection
    /// @param cli - pointer to websocket client
    /// @return ceammc_ws_rc::Ok, ceammc_ws_rc::InvalidClient, ceammc_ws_rc::CloseError,
    #[no_mangle]
    pub extern "C" fn ceammc_ws_client_close(cli: *mut ws_client) -> ws_rc {
        if cli.is_null() {
            return ws_rc::InvalidClient;
        }
        let cli = unsafe { &mut *cli };

        // can't write message after close call
        if !cli.ws.can_write() {
            return cli.err(ws_rc::CloseError, "connection already closed");
        }

        match cli.ws.close(None) {
            Ok(_) => ws_rc::Ok,
            Err(err) => cli.err(ws_rc::CloseError, format!("close error: {err}").as_str()),
        }
    }

    /// flush client connection
    /// @param cli - pointer to websocket client
    /// @return ceammc_ws_rc::Ok, ceammc_ws_rc::InvalidClient, ceammc_ws_rc::SendError,
    #[no_mangle]
    pub extern "C" fn ceammc_ws_client_flush(cli: *mut ws_client) -> ws_rc {
        if cli.is_null() {
            return ws_rc::InvalidClient;
        }
        let cli = unsafe { &mut *cli };

        // can't write message after close call
        if !cli.ws.can_write() {
            return cli.err(ws_rc::CloseError, "connection closed");
        }

        match cli.ws.flush() {
            Ok(_) => ws_rc::Ok,
            Err(err) => cli.err(ws_rc::SendError, format!("flush error: {err}").as_str()),
        }
    }
}
