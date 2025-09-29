use crate::server;
use std::{
    net::{TcpStream},
    time::{Instant},
    io::{Write},
    sync::{Arc, Mutex},
    sync::atomic::{AtomicI32, Ordering}
};
use server::person::Person;
use server::message::Message;
use serde_json::{json, Value};

pub fn handle_connections(
    mut stream: TcpStream,
    connected_people: Arc<Mutex<Vec<Person>>>,
    total_messages: Arc<Mutex<Vec<String>>>
)
{
    use std::io::Read;
    let mut buffer: [u8; 512] = [0u8; 512];

    loop
    {
        let bytes_read = match stream.read(&mut buffer)
        {
            Ok(0) => return,
            Ok(n) => n,
            Err(_) => return,
        };

        let raw_message: &[u8] = &buffer[..bytes_read];

        match serde_json::from_slice::<Message>(raw_message)
        {
            Ok(msg) =>
            {
                match msg
                {
                    Message::NewUser { username, user_color } =>
                    {
                        handle_new_user(user_color, username, &connected_people, &mut stream);
                    }
                    Message::Chat { user_id, content } =>
                    {
                        handle_user_message(user_id, content, &connected_people);
                    }
                    Message::Disconnect { user_id } =>
                    {
                        println!("User {} disconnected", user_id);
                        handle_disconnect_user(user_id, &connected_people);
                    }
                    Message::Command { message_type } =>
                    {
                        handle_commands(message_type, &mut stream);
                    }
                }
            }
            Err(e) =>
            {
                println!("Error parsing JSON: {}", e);
            }
        }
    }
}

fn assign_id() -> i32
{
    static ID: AtomicI32 = AtomicI32::new(0);
    ID.fetch_add(1, Ordering::Relaxed) + 1
}

fn handle_new_user
(
    color: u32,
    user: String,
    connected_people: &Arc<Mutex<Vec<Person>>>,
    stream: &mut TcpStream
)
{
    let mut people = connected_people.lock().unwrap();
    let id: i32 = assign_id();
    send_user_id(id, stream).unwrap();
    let new_user: Person = Person
    { user_color: (color), user_id: (id), user_name: (user.clone()), user_uptime: (Instant::now()) };
    println!("{} joined, assigned: {}", user, id);
    people.push(new_user);
}

fn handle_user_message
(
    user_id: i32,
    content: String,
    connected_people: &Arc<Mutex<Vec<Person>>>
)
{
    let people = connected_people.lock().unwrap();
    
    if let Some(index) = people.iter().position(|p| p.user_id == user_id)
    {
        println!("[{} with ID {}]: {}", people[index].user_name, user_id, content);
    }
    else
    {
        println!("[{} (unknown user)]: {}", user_id, content);
    }
}

fn send_user_id(assigned_id: i32, stream: &mut TcpStream) -> std::io::Result<()>
{
    let msg: serde_json::Value = json!({ "AssignedId": { "user_id": assigned_id } });
    let serialized = serde_json::to_string(&msg)
        .map_err(|e| std::io::Error::new(std::io::ErrorKind::Other, e))?;
    stream.write_all(serialized.as_bytes())?;
    Ok(())
}

fn handle_disconnect_user
(
    user_id: i32,
    connected_people: &Arc<Mutex<Vec<Person>>>
)
{
    let mut people = connected_people.lock().unwrap();
    people.retain(|p| p.user_id != user_id);
}

fn handle_commands(command_type: i32, stream: &mut TcpStream)
{
    match command_type
    {
        1 => { let _ = answer_ping(stream); }
        _ => { println!("Other"); }
    }
}

fn answer_ping(stream: &mut TcpStream) -> std::io::Result<()>
{
    let msg: serde_json::Value = json!({ "Ping": true });
    let serialized = serde_json::to_string(&msg)
        .map_err(|e| std::io::Error::new(std::io::ErrorKind::Other, e))?;
    stream.write_all(serialized.as_bytes())?;
   Ok(())
}