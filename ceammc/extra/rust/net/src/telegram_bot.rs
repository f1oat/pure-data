use frankenstein::{
    AsyncApi, AsyncTelegramApi, Audio, GetFileParams, GetUpdatesParams, Location, SendAudioParams,
    SendMessageParams, SendVoiceParams, UpdateContent, User, Voice,
};
use tokio::io::AsyncWriteExt;
use url::Url;

use crate::service::*;

use std::{
    ffi::{CStr, CString},
    os::raw::{c_char, c_double, c_void},
    path::Path,
    ptr::null_mut,
};

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct telegram_bot_init {
    /// telegram bot TOKEN
    token: *const c_char,
}

#[derive(Debug)]
enum TeleRequest {
    SendText(i64, i32, String),
    SendAudio(i64, String),
    SendVoice(i64, String),
    GetFile(String, String),
    Whoami,
    Logout,
    Quit,
}

#[derive(Debug)]
pub enum TeleReply {
    Whoami(User),
    Text(i64, i32, String),
    Location(i64, Location),
    Sticker(i64, String, String),
    Voice(i64, Voice),
    Audio(i64, Audio),
    Photo,
}

type TeleService = Service<TeleRequest, TeleReply>;

#[allow(non_camel_case_types)]
pub struct telegram_bot_client {
    cli: TeleService,
}

impl Drop for telegram_bot_client {
    fn drop(&mut self) {
        self.cli.blocking_send(TeleRequest::Quit);
    }
}

#[allow(non_camel_case_types)]
#[repr(C)]
pub struct telegram_bot_result_cb {
    /// user data pointer (can be NULL)
    user: *mut c_void,
    /// connection callback function (can be NULL)
    whoami_cb: Option<extern "C" fn(user: *mut c_void, user_id: u64, username: *const c_char)>,
    /// text callback function (can be NULL)
    text_cb:
        Option<extern "C" fn(user: *mut c_void, chat_id: i64, msg_id: i32, text: *const c_char)>,
    /// location callback function (can be NULL)
    loc_cb: Option<
        extern "C" fn(user: *mut c_void, chat_id: i64, latitude: c_double, longitude: c_double),
    >,
    /// sticker callback function (can be NULL)
    sti_cb: Option<
        extern "C" fn(
            user: *mut c_void,
            chat_id: i64,
            file_id: *const c_char,
            emoji: *const c_char,
        ),
    >,
    /// voice callback function (can be NULL)
    voice_cb: Option<
        extern "C" fn(
            user: *mut c_void,
            chat_id: i64,
            file_id: *const c_char,
            file_unique_id: *const c_char,
            mime: *const c_char,
            file_duration: u32,
            file_size: u64,
        ),
    >,
    /// voice callback function (can be NULL)
    audio_cb: Option<
        extern "C" fn(
            user: *mut c_void,
            chat_id: i64,
            file_id: *const c_char,
            file_unique_id: *const c_char,
            mime: *const c_char,
            file_name: *const c_char,
            file_duration: u32,
            file_size: u64,
            title: *const c_char,
        ),
    >,
}

impl ServiceCallback<TeleReply> for telegram_bot_result_cb {
    fn exec(&self, reply: &mut TeleReply) {
        match reply {
            TeleReply::Text(chat_id, msg_id, txt) => {
                let txt = CString::new(txt.clone()).unwrap_or_default();
                self.text_cb
                    .map(|f| f(self.user, *chat_id, *msg_id, txt.as_ptr()));
            }
            TeleReply::Photo => todo!(),
            TeleReply::Whoami(user) => {
                let name = CString::new(user.first_name.clone()).unwrap_or_default();
                self.whoami_cb.map(|f| f(self.user, user.id, name.as_ptr()));
            }
            TeleReply::Location(chat_id, location) => {
                self.loc_cb
                    .map(|f| f(self.user, *chat_id, location.latitude, location.longitude));
            }
            TeleReply::Sticker(chat_id, file_id, emoji) => {
                let file_id = CString::new(file_id.clone()).unwrap_or_default();
                let emoji = CString::new(emoji.clone()).unwrap_or_default();

                self.sti_cb
                    .map(|f| f(self.user, *chat_id, file_id.as_ptr(), emoji.as_ptr()));
            }
            TeleReply::Voice(chat_id, voice) => {
                let file_id = CString::new(voice.file_id.clone()).unwrap_or_default();
                let file_unique_id = CString::new(voice.file_unique_id.clone()).unwrap_or_default();
                let mime_type =
                    CString::new(voice.mime_type.clone().unwrap_or_default()).unwrap_or_default();

                self.voice_cb.map(|f| {
                    f(
                        self.user,
                        *chat_id,
                        file_id.as_ptr(),
                        file_unique_id.as_ptr(),
                        mime_type.as_ptr(),
                        voice.duration,
                        voice.file_size.unwrap_or_default(),
                    )
                });
            }
            TeleReply::Audio(chat_id, audio) => {
                let file_id = CString::new(audio.file_id.clone()).unwrap_or_default();
                let file_unique_id = CString::new(audio.file_unique_id.clone()).unwrap_or_default();
                let file_name =
                    CString::new(audio.file_name.clone().unwrap_or_default()).unwrap_or_default();
                let title =
                    CString::new(audio.title.clone().unwrap_or_default()).unwrap_or_default();
                let mime_type =
                    CString::new(audio.mime_type.clone().unwrap_or_default()).unwrap_or_default();

                self.audio_cb.map(|f| {
                    f(
                        self.user,
                        *chat_id,
                        file_id.as_ptr(),
                        file_unique_id.as_ptr(),
                        mime_type.as_ptr(),
                        file_name.as_ptr(),
                        audio.duration,
                        audio.file_size.unwrap_or_default(),
                        title.as_ptr(),
                    )
                });
            }
        }
    }
}

#[must_use]
fn make_file_url(api_url: &str, file: &str) -> Option<String> {
    Url::parse(api_url).ok().and_then(|mut api_url| {
        let api_path = api_url.path_segments()?.next()?.to_owned();

        api_url
            .path_segments_mut()
            .ok()?
            .pop()
            .push("file")
            .push(api_path.as_str())
            .push(file);

        Some(api_url.as_str().to_owned())
    })
}

#[must_use]
fn make_file_full_path(base_dir: &str, path: &str) -> Option<String> {
    let base_dir = Path::new(base_dir);
    let fname = Path::new(path);
    let filename = fname.file_name()?.to_str()?;
    if fname.is_absolute() {
        Some(filename.to_owned())
    } else {
        Some(
            base_dir
                .join(&fname.file_name().unwrap())
                .as_path()
                .to_str()?
                .to_owned(),
        )
    }
}

#[no_mangle]
/// create new telegram bot
/// @param params - telegram init settings
/// @param cb_err - callback called on error
/// @param cb_post - callback called on post
/// @param cb_debug - callback called on debug
/// @param cb_log - callback called on log
/// @param _cb_progress - not used
/// @param cb_reply - reply callback
/// @param cb_notify - callback called to notify that it's time to check results
/// @return pointer to telegram bot handle or NULL on error
pub extern "C" fn ceammc_telegram_bot_new(
    params: telegram_bot_init,
    cb_err: callback_msg,
    cb_post: callback_msg,
    cb_debug: callback_msg,
    cb_log: callback_msg,
    _cb_progress: callback_progress,
    cb_reply: telegram_bot_result_cb,
    cb_notify: callback_notify,
) -> *mut telegram_bot_client {
    if params.token.is_null() {
        cb_err.exec("invalid token");
        return std::ptr::null_mut();
    }
    let token = unsafe { CStr::from_ptr(params.token) }.to_str();
    if token.is_err() {
        cb_err.exec("invalid token string");
        return std::ptr::null_mut();
    }

    let rt = tokio::runtime::Runtime::new();

    async fn send_error(
        cb: &callback_notify,
        rep_tx: &tokio::sync::mpsc::Sender<Result<TeleReply, Error>>,
        msg: String,
    ) {
        if let Err(err) = rep_tx.send(Err(crate::service::Error::Error(msg))).await {
            eprintln!("send error: {err}");
        } else {
            cb.exec();
        }
    }

    async fn send_debug(
        cb: &callback_notify,
        rep_tx: &tokio::sync::mpsc::Sender<Result<TeleReply, Error>>,
        msg: String,
    ) {
        if let Err(err) = rep_tx.send(Err(crate::service::Error::Debug(msg))).await {
            eprintln!("debug error: {err}");
        } else {
            cb.exec();
        }
    }

    async fn send_reply(
        cb: &callback_notify,
        rep_tx: &tokio::sync::mpsc::Sender<Result<TeleReply, Error>>,
        msg: TeleReply,
    ) {
        if let Err(err) = rep_tx.send(Ok(msg)).await {
            eprintln!("send error: {err}");
        } else {
            cb.exec();
        }
    }

    async fn proc_requests(
        var: Option<TeleRequest>,
        api: &AsyncApi,
        cb_notify: callback_notify,
        rep_tx: &tokio::sync::mpsc::Sender<Result<TeleReply, Error>>,
    ) -> bool {
        match var {
            Some(req) => {
                match req {
                    TeleRequest::SendText(chat_id, _msg_id, msg) => {
                        // let reply_parameters = ReplyParameters::builder()
                        // .message_id(message.message_id)
                        // .build();

                        let send_message_params = SendMessageParams::builder()
                            .chat_id(chat_id)
                            .text(msg)
                            // .reply_parameters(reply_parameters)
                            .build();

                        if let Err(err) = api.send_message(&send_message_params).await {
                            send_error(&cb_notify, &rep_tx, format!("send message error: {err}"))
                                .await;
                        }
                        true
                    }
                    TeleRequest::GetFile(file_id, base_dir) => {
                        let params = GetFileParams::builder().file_id(file_id).build();

                        match api.get_file(&params).await {
                            Ok(m) => {
                                if m.ok {
                                    if let Some(path) = m.result.file_path {
                                        let file_url = make_file_url(&api.api_url, path.as_str());
                                        let req = api
                                            .client
                                            .get(file_url.unwrap_or_default())
                                            .send()
                                            .await;

                                        match req {
                                            Ok(mut response) => {
                                                if !response.status().is_success() {
                                                    send_error(
                                                        &cb_notify,
                                                        &rep_tx,
                                                        format!(
                                                            "{}: {}",
                                                            response.status(),
                                                            response.url()
                                                        ),
                                                    )
                                                    .await;
                                                    return true;
                                                }

                                                let full_name = make_file_full_path(
                                                    &base_dir.as_str(),
                                                    path.as_str(),
                                                )
                                                .unwrap_or_default();

                                                eprintln!("full_name: {full_name}");

                                                match tokio::fs::File::create(&full_name).await {
                                                    Ok(mut file) => {
                                                        let total = m.result.file_size.unwrap_or(0);
                                                        let mut bytes_send: u64 = 0;

                                                        while let Ok(chunk) = response.chunk().await
                                                        {
                                                            if let Some(bytes) = chunk {
                                                                if let Err(err) =
                                                                    file.write_all(&bytes).await
                                                                {
                                                                    send_error(&cb_notify, &rep_tx, format!("can't write to file: {err}")).await;
                                                                    return true;
                                                                }

                                                                if total > 0 {
                                                                    bytes_send +=
                                                                        bytes.len() as u64;
                                                                    let perc =
                                                                        (100 * bytes_send) / total;
                                                                    eprintln!("done: {perc}%");
                                                                    if !TeleService::write_progress(
                                                                        &rep_tx, perc as u8,
                                                                        cb_notify,
                                                                    )
                                                                    .await
                                                                    {
                                                                        break;
                                                                    }
                                                                }
                                                            } else {
                                                                break;
                                                            }
                                                        }

                                                        if let Err(err) = file.flush().await {
                                                            send_error(
                                                                &cb_notify,
                                                                &rep_tx,
                                                                format!("file flush error: {err}"),
                                                            )
                                                            .await;
                                                            return true;
                                                        } else {
                                                            send_debug(&cb_notify, &rep_tx, format!("file downloaded to '{full_name}'")).await;
                                                            return true;
                                                        }
                                                    }
                                                    Err(err) => {
                                                        send_error(
                                                            &cb_notify,
                                                            &rep_tx,
                                                            format!("file create error: {err}"),
                                                        )
                                                        .await;
                                                        return true;
                                                    }
                                                }
                                            }
                                            Err(err) => {
                                                send_error(
                                                    &cb_notify,
                                                    &rep_tx,
                                                    format!("get file error: {err}"),
                                                )
                                                .await;
                                            }
                                        }
                                    } else {
                                        send_error(
                                            &cb_notify,
                                            &rep_tx,
                                            format!("can't get file path"),
                                        )
                                        .await;
                                    }

                                    // writing to a custom file
                                    // with open("custom/file.doc", 'wb') as f:
                                    // context.bot.get_file(update.message.document).download(out=f)
                                } else {
                                    send_error(&cb_notify, &rep_tx, format!("can't get file"))
                                        .await;
                                }
                            }
                            Err(err) => {
                                send_error(&cb_notify, &rep_tx, format!("get file error: {err}"))
                                    .await;
                            }
                        }
                        true
                    }
                    TeleRequest::SendAudio(chat_id, file_path) => {
                        let file = std::path::PathBuf::from(file_path);
                        let params = SendAudioParams::builder()
                            .chat_id(chat_id)
                            .audio(file)
                            .build();

                        match api.send_audio(&params).await {
                            Ok(m) => {
                                if m.ok {
                                } else {
                                    send_error(&cb_notify, &rep_tx, format!("can't send audio"))
                                        .await;
                                }
                            }
                            Err(err) => {
                                send_error(&cb_notify, &rep_tx, format!("send audio error: {err}"))
                                    .await;
                            }
                        }
                        true
                    }
                    TeleRequest::SendVoice(chat_id, file_path) => {
                        let file = std::path::PathBuf::from(file_path);
                        let params = SendVoiceParams::builder()
                            .chat_id(chat_id)
                            .voice(file)
                            .build();

                        match api.send_voice(&params).await {
                            Ok(m) => {
                                if m.ok {
                                } else {
                                    send_error(&cb_notify, &rep_tx, format!("can't send voice"))
                                        .await;
                                }
                            }
                            Err(err) => {
                                send_error(&cb_notify, &rep_tx, format!("send voice error: {err}"))
                                    .await;
                            }
                        }
                        true
                    }
                    TeleRequest::Whoami => {
                        match api.get_me().await {
                            Ok(user) => {
                                send_reply(&cb_notify, &rep_tx, TeleReply::Whoami(user.result))
                                    .await
                            }
                            Err(err) => {
                                send_error(&cb_notify, &rep_tx, format!("whoami error: {err}"))
                                    .await
                            }
                        }
                        true
                    }
                    TeleRequest::Logout => {
                        if let Err(err) = api.log_out().await {
                            send_error(&cb_notify, &rep_tx, format!("logout error: {err}")).await;
                        }
                        true
                    }
                    TeleRequest::Quit => false,
                }
            }
            None => false,
        }
    }

    async fn proc_events(
        content: &UpdateContent,
        cb_notify: callback_notify,
        rep_tx: &tokio::sync::mpsc::Sender<Result<TeleReply, Error>>,
    ) {
        match content {
            UpdateContent::Message(msg) => {
                if let Some(text) = &msg.text {
                    eprintln!("text '{text}' from #{:X}", msg.chat.id);
                    send_reply(
                        &cb_notify,
                        &rep_tx,
                        TeleReply::Text(msg.chat.id, msg.message_id, text.clone()),
                    )
                    .await;
                }

                if let Some(location) = &msg.location {
                    eprintln!("location {location:?} from #{:X}", msg.chat.id);
                    send_reply(
                        &cb_notify,
                        &rep_tx,
                        TeleReply::Location(msg.chat.id, **location),
                    )
                    .await;
                }

                if let Some(sticker) = &msg.sticker {
                    eprintln!("sticker {sticker:?} from #{:X}", msg.chat.id);
                    send_reply(
                        &cb_notify,
                        &rep_tx,
                        TeleReply::Sticker(
                            msg.chat.id,
                            sticker.file_id.clone(),
                            sticker.emoji.clone().unwrap_or_default(),
                        ),
                    )
                    .await;
                }

                if let Some(voice) = &msg.voice {
                    eprintln!("voice {voice:?} from #{:X}", msg.chat.id);
                    send_reply(
                        &cb_notify,
                        &rep_tx,
                        TeleReply::Voice(msg.chat.id, *voice.clone()),
                    )
                    .await;
                }

                if let Some(audio) = &msg.audio {
                    eprintln!("audio {audio:?} from #{:X}", msg.chat.id);
                    send_reply(
                        &cb_notify,
                        &rep_tx,
                        TeleReply::Audio(msg.chat.id, *audio.clone()),
                    )
                    .await;
                }
            }
            // UpdateContent::EditedMessage(_) => todo!(),
            // UpdateContent::ChannelPost(_) => todo!(),
            // UpdateContent::EditedChannelPost(_) => todo!(),
            // UpdateContent::MessageReaction(_) => todo!(),
            // UpdateContent::MessageReactionCount(_) => todo!(),
            // UpdateContent::InlineQuery(_) => todo!(),
            // UpdateContent::ChosenInlineResult(_) => todo!(),
            // UpdateContent::CallbackQuery(_) => todo!(),
            // UpdateContent::ShippingQuery(_) => todo!(),
            // UpdateContent::PreCheckoutQuery(_) => todo!(),
            // UpdateContent::Poll(_) => todo!(),
            // UpdateContent::PollAnswer(_) => todo!(),
            // UpdateContent::MyChatMember(_) => todo!(),
            // UpdateContent::ChatMember(_) => todo!(),
            // UpdateContent::ChatJoinRequest(_) => todo!(),
            // UpdateContent::ChatBoost(_) => todo!(),
            // UpdateContent::RemovedChatBoost(_) => todo!(),
            _x => {
                eprintln!("content: {_x:?}");
            }
        }
    }

    match rt {
        Ok(rt) => {
            let api = AsyncApi::new(token.unwrap());
            let update_params_builder = GetUpdatesParams::builder();
            let mut update_params = update_params_builder.clone().build();

            let (req_tx, mut req_rx) = tokio::sync::mpsc::channel::<TeleRequest>(32);
            let (rep_tx, rep_rx) =
                tokio::sync::mpsc::channel::<Result<TeleReply, crate::service::Error>>(16);

            std::thread::spawn(move || {
                rt.block_on(async move {
                    loop {
                        tokio::select! {
                            // process requests
                            var = req_rx.recv() => {
                                if !proc_requests(var, &api, cb_notify, &rep_tx).await {
                                    return ();
                                }
                            },
                            // process telegram events
                            val = api.get_updates(&update_params) => {
                                match val {
                                    Ok(response) => {
                                        for update in response.result {
                                            proc_events(&update.content, cb_notify, &rep_tx).await;

                                            update_params = update_params_builder
                                                .clone()
                                                .offset(update.update_id + 1)
                                                .build();
                                        }
                                    },
                                    Err(err) =>  {
                                        send_error(&cb_notify, &rep_tx, err.to_string()).await;
                                        break;
                                    },
                                }

                            },
                        }
                    }
                });
            });

            let srv = Service::<TeleRequest, TeleReply>::new(
                cb_err,
                cb_post,
                cb_debug,
                cb_log,
                callback_progress::default(),
                Box::new(cb_reply),
                req_tx,
                rep_rx,
            );

            return Box::into_raw(Box::new(telegram_bot_client { cli: srv }));
        }
        Err(err) => {
            cb_err.exec(format!("tokio runtime creation error: {err}").as_str());
            return null_mut();
        }
    }
}

#[no_mangle]
/// free telegram bot
/// @param cli - pointer to telegram bot
pub extern "C" fn ceammc_telegram_bot_free(cli: *mut telegram_bot_client) {
    if !cli.is_null() {
        drop(unsafe { Box::from_raw(cli) });
    }
}

#[no_mangle]
/// process telegram events
/// @param cli - pointer to telegram bot
pub extern "C" fn ceammc_telegram_bot_process(cli: *mut telegram_bot_client) -> bool {
    if cli.is_null() {
        return false;
    }
    let cli = unsafe { &mut *cli };
    cli.cli.process_results();
    true
}

#[no_mangle]
/// logout telegram bot
/// @param cli - pointer to telegram bot
pub extern "C" fn ceammc_telegram_bot_logout(cli: *mut telegram_bot_client) -> bool {
    if cli.is_null() {
        return false;
    }
    let cli = unsafe { &*cli };
    return cli.cli.send_request(TeleRequest::Logout);
}

#[no_mangle]
/// send text message from telegram bot
/// @param cli - pointer to telegram bot
/// @param chat_id - target chat id
/// @param msg_id - reply message id
/// @param text - message text
/// @return true on success
pub extern "C" fn ceammc_telegram_bot_send_message(
    cli: *mut telegram_bot_client,
    chat_id: i64,
    msg_id: i32,
    text: *const c_char,
) -> bool {
    if cli.is_null() {
        return false;
    }
    let cli = unsafe { &*cli };

    if text.is_null() {
        cli.cli.on_error("NULL text message");
        return false;
    }
    let text = unsafe { CStr::from_ptr(text) }
        .to_str()
        .unwrap_or_default()
        .to_owned();

    return cli
        .cli
        .send_request(TeleRequest::SendText(chat_id, msg_id, text));
}

#[no_mangle]
/// send text message from telegram bot
/// @param cli - pointer to telegram bot
pub extern "C" fn ceammc_telegram_bot_whoami(cli: *mut telegram_bot_client) -> bool {
    if cli.is_null() {
        return false;
    }
    let cli = unsafe { &*cli };

    return cli.cli.send_request(TeleRequest::Whoami);
}

#[no_mangle]
/// get file from telegram bot
/// @param cli - pointer to telegram bot
pub extern "C" fn ceammc_telegram_bot_get_file(
    cli: *mut telegram_bot_client,
    file_id: *const c_char,
    base_dir: *const c_char,
) -> bool {
    if cli.is_null() {
        return false;
    }
    let cli = unsafe { &*cli };

    if file_id.is_null() {
        cli.cli.on_error("NULL file id");
        return false;
    }
    let file_id = unsafe { CStr::from_ptr(file_id) }
        .to_str()
        .unwrap_or_default()
        .to_owned();

    if base_dir.is_null() {
        cli.cli.on_error("NULL base_dir");
        return false;
    }
    let base_dir = unsafe { CStr::from_ptr(base_dir) }
        .to_str()
        .unwrap_or_default()
        .to_owned();

    return cli
        .cli
        .send_request(TeleRequest::GetFile(file_id, base_dir));
}

#[no_mangle]
/// send audio from telegram bot
/// @param cli - pointer to telegram bot
/// @param chat_id - target chat id
/// @param file - full path to the file
pub extern "C" fn ceammc_telegram_bot_send_audio(
    cli: *mut telegram_bot_client,
    chat_id: i64,
    file: *const c_char,
) -> bool {
    if cli.is_null() {
        return false;
    }
    let cli = unsafe { &*cli };

    if file.is_null() {
        cli.cli.on_error("NULL file");
        return false;
    }
    let file = unsafe { CStr::from_ptr(file) }
        .to_str()
        .unwrap_or_default()
        .to_owned();

    return cli.cli.send_request(TeleRequest::SendAudio(chat_id, file));
}

#[no_mangle]
/// send voice from telegram bot
/// @param cli - pointer to telegram bot
/// @param chat_id - target chat id
/// @param file - full path to the file
pub extern "C" fn ceammc_telegram_bot_send_voice(
    cli: *mut telegram_bot_client,
    chat_id: i64,
    file: *const c_char,
) -> bool {
    if cli.is_null() {
        return false;
    }
    let cli = unsafe { &*cli };

    if file.is_null() {
        cli.cli.on_error("NULL file");
        return false;
    }
    let file = unsafe { CStr::from_ptr(file) }
        .to_str()
        .unwrap_or_default()
        .to_owned();

    return cli.cli.send_request(TeleRequest::SendVoice(chat_id, file));
}
