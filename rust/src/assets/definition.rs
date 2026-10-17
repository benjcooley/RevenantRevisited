// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             definition.rs - Quoted DEF lines and continuations        *
// *************************************************************************

use anyhow::{Result, ensure};

/// Preserve quoted strings while stripping comments and joining continued
/// lines. Retail definitions use single-byte text and CRLF line endings.
pub fn lines(bytes: &[u8]) -> Result<Vec<Vec<String>>> {
    let mut out = Vec::new();
    let mut line = Vec::new();
    let mut token = String::new();
    let mut quoted = false;
    let mut block = false;
    let mut i = 0;
    while i < bytes.len() {
        let c = bytes[i];
        if block {
            if bytes.get(i..i + 2) == Some(b"*/") {
                block = false;
                i += 2;
            } else {
                i += 1;
            }
            continue;
        }
        if c == b'"' {
            if quoted {
                line.push(std::mem::take(&mut token));
            }
            quoted = !quoted;
        } else if quoted {
            token.push(char::from(c));
        } else if bytes.get(i..i + 2) == Some(b"/*") {
            block = true;
            i += 2;
            continue;
        } else if bytes.get(i..i + 2) == Some(b"//") {
            while i < bytes.len() && bytes[i] != b'\n' {
                i += 1;
            }
            continue;
        } else if c == b'\\' {
            if !token.is_empty() {
                line.push(std::mem::take(&mut token));
            }
            let mut end = i + 1;
            while end < bytes.len() && matches!(bytes[end], b' ' | b'\t' | b'\r') {
                end += 1;
            }
            if bytes.get(end) == Some(&b'\n') {
                i = end + 1;
                continue;
            }
        } else if c.is_ascii_whitespace() {
            if !token.is_empty() {
                line.push(std::mem::take(&mut token));
            }
            if c == b'\n' && !line.is_empty() {
                out.push(std::mem::take(&mut line));
            }
        } else {
            token.push(char::from(c));
        }
        i += 1;
    }
    ensure!(!quoted && !block, "unterminated DEF quote/comment");
    if !token.is_empty() {
        line.push(token);
    }
    if !line.is_empty() {
        out.push(line);
    }
    Ok(out)
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn comments_and_continuations_respect_quoted_literals() {
        let input = b"/* hidden\nBUTTON nope */ PANEL \"default\"\r\nBUTTON TEXT \"//keep /* this */\" \\\r\n POS 1 2 3 4 // trailing\r\n";
        assert_eq!(
            lines(input).unwrap(),
            [
                vec!["PANEL", "default"],
                vec![
                    "BUTTON",
                    "TEXT",
                    "//keep /* this */",
                    "POS",
                    "1",
                    "2",
                    "3",
                    "4"
                ]
            ]
        );
        assert!(lines(b"TEXT \"unfinished").is_err());
    }
}
