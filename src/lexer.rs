use std::fs::OpenOptions;
use std::io::prelude::*;
use std::str;
use std::collections::VecDeque;
use std::path;
use std::process;
use std::collections::{HashSet, HashMap};
use token;

pub struct Lexer {
    pub peek: String,
    pub peek_pos: usize,
    pub cur_pos: token::Pos,
    pub buf: VecDeque<VecDeque<token::Token>>,
}
