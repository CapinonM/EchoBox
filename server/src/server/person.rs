use std::time::Instant;

pub struct Person
{
    pub user_color: u32,
    pub user_id: i32,
    pub user_name: String,
    pub user_uptime: Instant,
}

impl Person
{
    pub fn new(user_color: u32, user_id: i32, user_name: String) -> Self
    {
        Person
        {
            user_color,
            user_id,
            user_name,
            user_uptime: Instant::now(),
        }
    }
    pub fn time_since_join(&self) -> std::time::Duration
    {
        Instant::now().duration_since(self.user_uptime)
    }
}