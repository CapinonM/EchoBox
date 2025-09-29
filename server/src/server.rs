mod person;
mod message;
mod handlers;

use std::{
    net::{TcpListener},
    sync::{Arc, Mutex}
};
use person::Person;
use handlers::handle_connections;

pub fn run()
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

        std::thread::spawn(move || {
            handle_connections(stream, connected_people, total_messages);
        });
    }
}