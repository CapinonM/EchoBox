use serde::{Serialize, Deserialize};

#[derive(Serialize, Deserialize, Debug)]
pub enum Message
{
    NewUser { username: String, user_color: u32 },
    Chat { user_id: i32, content: String },
    Disconnect { user_id: i32 },
    Command { message_type: i32 },
}