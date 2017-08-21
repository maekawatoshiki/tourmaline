pub struct Token {
    pub kind: Kind,
    pub pos: Pos,
}

pub enum Kind {
    Keyword(Keyword),
    Identifier(String),
    Number(String),
    String(String),
    Symbol(Symbol),
    Delimiter,
}

pub enum Keyword {
}

pub enum Symbol {
}

pub struct Pos {
    line: usize,
    column: usize,
}
