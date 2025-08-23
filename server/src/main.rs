use std::{
    io::{Read,Write},
    net::{TcpListener, TcpStream},
    sync::
    {
        Arc, Mutex,
        atomic::{AtomicI32, Ordering}
    },
    thread,
    time::Duration,
    time::Instant
};

use serde::{
    Serialize,
    Deserialize
};


#[derive(Serialize, Deserialize, Debug)]
enum Message {
    NewUser { username: String, user_color: u32 },
    Chat { user_id: i32, content: String },
    Disconnect { user_id: i32 }
}

struct Person 
{
    user_color: u32, // Actually not implemented in client, is saved in hex
    user_id: i32,
    user_name: String,
    user_uptime: Instant
}

impl Person
{
    fn new(user_color: u32, user_id: i32,
    user_name: String, user_uptime: Instant) -> Self
    {
        Person
        {
            user_color,
            user_id,
            user_name,
            user_uptime: Instant::now(),
        }
    }
    fn time_since_join(&self) -> std::time::Duration
    {
        Instant::now().duration_since(self.user_uptime)
    }
}

fn main()
{
    let listener = TcpListener::bind("127.0.0.1:7878").unwrap();
    println!("Waiting for connections...");
    let connected_people = Arc::new(Mutex::new(Vec::<Person>::new()));
    let total_messages = Arc::new(Mutex::new(Vec::<String>::new()));

    for stream in listener.incoming()
    {
        let stream = stream.unwrap();
        let connected_people = Arc::clone(&connected_people);
        let total_messages = Arc::clone(&total_messages);

        thread::spawn(move || 
        {
            handle_connections(stream, connected_people, total_messages);
        });
    }
}

fn handle_connections(
    mut stream: TcpStream,
    connected_people: Arc<Mutex<Vec<Person>>>,
    total_messages: Arc<Mutex<Vec<String>>>
)
{
    let mut buffer = [0u8; 512];

    loop
    {
        let bytes_read = match stream.read(&mut buffer)
        {
            Ok(0) => return,
            Ok(n) => n,
            Err(_) => return,
        };

        let raw_message = &buffer[..bytes_read];

        match serde_json::from_slice::<Message>(raw_message)
        {
            Ok(msg) =>
            {
                match msg
                {
                    Message::NewUser { username, user_color } =>
                    {
                        handle_new_user(user_color, username, &connected_people);
                    }
                    Message::Chat { user_id, content } =>
                    {
                        println!("[{}]: {}", user_id, content);
                    }
                    Message::Disconnect { user_id } =>
                    {
                        println!("User {} disconnected", user_id);
                        handle_disconnect_user(user_id, &connected_people);
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
    connected_people: &Arc<Mutex<Vec<Person>>>
)
{
    let mut people = connected_people.lock().unwrap();
    let id: i32 = assign_id();
    let new_user = Person
    {
        user_color: color,
        user_id: id,
        user_name: user.clone(),
        user_uptime: Instant::now()
    };
    println!("{} joined, assigned: {}", user, id);
    people.push(new_user);
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
