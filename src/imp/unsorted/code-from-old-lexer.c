/* --------------------------------------------------------------------------
 * Forward declarations
 * ----------------------------------------------------------------------- */

static void get_new_lookahead_sym (m2c_lexer_t lexer);

static char skip_code_section (m2c_lexer_t lexer);

static char skip_line_comment (m2c_lexer_t lexer);

static char skip_block_comment (m2c_lexer_t lexer);

static char get_pragma(m2c_lexer_t lexer);

static char get_ident (m2c_lexer_t lexer);

static char get_ident_or_resword (m2c_lexer_t lexer, m2c_token_t *token);

static char get_string_literal (m2c_lexer_t lexer, m2c_token_t *token);

static char get_number_literal
  (m2c_lexer_t lexer, m2c_token_t *token);
  
static char get_number_literal_fractional_part
  (m2c_lexer_t lexer, m2c_token_t *token);


/* --------------------------------------------------------------------------
 * private function skip_code_section(lexer)
 * ----------------------------------------------------------------------- */

#define IS_EOF(_lexer,_ch) \
  (((_ch) == ASCII_EOT) && \
   (m2c_infile_status(_lexer->infile) == \
    M2C_INFILE_STATUS_ATTEMPT_TO_READ_PAST_EOF))

static char skip_code_section (m2c_lexer_t lexer) {
  
  bool delimiter_found = false;
  uint_t first_line;
  char next_char;
  
  /* remember line number for warning */
  first_line = m2c_infile_current_line(lexer->infile);
  
  /* consume opening '?' and '<' */
  next_char = m2c_consume_char(lexer->infile);
  next_char = m2c_consume_char(lexer->infile);
  
  while ((!delimiter_found) && (!IS_EOF(lexer, next_char))) {
    
    /* check for closing delimiter */
    if ((next_char == '>') && (m2c_la2_char(lexer->infile) == '?') &&
       /* first column */ (m2c_infile_current_column(lexer->infile) == 1)) {
      
      /* closing delimiter */
      delimiter_found = true;
        
      /* consume closing '>' and '?' */
      next_char = m2c_consume_char(lexer->infile);
      next_char = m2c_consume_char(lexer->infile);
      break;
    } /* end if */
    
    /* check for illegal control characters */
    if ((IS_CONTROL_CHAR(next_char)) &&
        (next_char != ASCII_TAB) &&
        (next_char != ASCII_LF)) {
      /* invalid input character */
      report_error_w_offending_char
        (M2C_ERROR_INVALID_INPUT_CHAR, lexer,
         m2c_infile_current_line(lexer->infile),
         m2c_infile_current_column(lexer->infile), next_char);
    } /* end if */
    
    next_char = m2c_consume_char(lexer->infile);
  } /* end while */
  
  /* disabled code section warning */
  m2c_emit_warning_w_range
    (M2C_WARN_DISABLED_CODE_SECTION,
     first_line, m2c_infile_current_line(lexer->infile));
  
  return next_char;
} /* end skip_code_section */


/* --------------------------------------------------------------------------
 * private function skip_line_comment(lexer)
 * ----------------------------------------------------------------------- */

static char skip_line_comment(m2c_lexer_t lexer) {
  
  char next_char;

  /* consume opening '!' */
  next_char = m2c_consume_char(lexer->infile);
  
  while ((next_char != ASCII_LF) && (!IS_EOF(lexer, next_char))) {
    
    /* check for illegal characters */
    if (IS_CONTROL_CHAR(next_char) && (next_char != ASCII_TAB)) {
      /* invalid input character */
      report_error_w_offending_char
        (M2C_ERROR_INVALID_INPUT_CHAR, lexer,
         m2c_infile_current_line(lexer->infile),
         m2c_infile_current_column(lexer->infile), next_char);
    } /* end if */
  
    next_char = m2c_consume_char(lexer->infile);
  } /* end while */
  
  return next_char;
} /* end skip_line_comment */


/* --------------------------------------------------------------------------
 * private function skip_block_comment(lexer)
 * ----------------------------------------------------------------------- */

static char skip_block_comment(m2c_lexer_t lexer) {
  
  uint_t line, column, comment_nesting_level = 1;
  char next_char;
  
  /* consume opening '(' and '*' */
  next_char = m2c_consume_char(lexer->infile);
  next_char = m2c_consume_char(lexer->infile);
  
  while (comment_nesting_level > 0) {
    
    /* check for opening block comment */
    if (next_char == '(') {
      next_char = m2c_consume_char(lexer->infile);
      if (next_char == '*') {
        next_char = m2c_consume_char(lexer->infile);
        comment_nesting_level++;
      } /* end if */
    }
    
    /* check for closing block comment */
    else if (next_char == '*') {
      next_char = m2c_consume_char(lexer->infile);
      if (next_char == ')') {
        next_char = m2c_consume_char(lexer->infile);
        comment_nesting_level--;
      } /* end if */
    }
    
    /* other characters permitted within block comments */
    else if ((!IS_CONTROL_CHAR(next_char)) ||
             (next_char == ASCII_TAB) ||
             (next_char == ASCII_LF)) {
      next_char = m2c_consume_char(lexer->infile);
    }
    
    else /* error */ {
      line = m2c_infile_current_line(lexer->infile);
      column = m2c_infile_current_column(lexer->infile);
      
      /* end-of-file reached */
      if (IS_EOF(lexer, next_char)) {
        report_error_w_offending_pos
          (M2C_ERROR_EOF_IN_BLOCK_COMMENT, lexer,  line, column);
      }
      else /* illegal character */ {
        report_error_w_offending_char
          (M2C_ERROR_INVALID_INPUT_CHAR, lexer,  line, column, next_char);
        next_char = m2c_consume_char(lexer->infile);
      } /* end if */
    } /* end if */
  } /* end while */
  
  return next_char;
} /* end skip_block_comment */


/* --------------------------------------------------------------------------
 * private function get_pragma(lexer)
 * ----------------------------------------------------------------------- */

static char get_pragma(m2c_lexer_t lexer) {
  
  bool delimiter_found = false;
  uint_t line, column;
  char next_char;
  
  m2c_mark_lexeme(lexer->infile);
  
  /* consume opening '<' and '*' */
  next_char = m2c_consume_char(lexer->infile);
  next_char = m2c_consume_char(lexer->infile);
    
  while (!delimiter_found) {
    
    if /* closing delimiter */
      ((next_char == '*') && (m2c_la2_char(lexer->infile) == '>')) {
      
      delimiter_found = true;
            
      /* consume closing '*' and '>' */
      next_char = m2c_consume_char(lexer->infile);
      next_char = m2c_consume_char(lexer->infile);
      
      /* get lexeme */
      lexer->lookahead.lexeme = m2c_read_marked_lexeme(lexer->infile);
    }
    
    /* other non-control characters */
    else if (!IS_CONTROL_CHAR(next_char)) {
      next_char = m2c_consume_char(lexer->infile);
    }
    
    else /* error */ {
      line = m2c_infile_current_line(lexer->infile);
      column = m2c_infile_current_column(lexer->infile);
      
      /* end-of-file reached */
      if (IS_EOF(lexer, next_char)) {
        report_error_w_offending_pos
          (M2C_ERROR_EOF_IN_PRAGMA, lexer,  line, column);
      }
      else /* illegal character */ {
        report_error_w_offending_char
          (M2C_ERROR_INVALID_INPUT_CHAR, lexer,  line, column, next_char);
        next_char = m2c_consume_char(lexer->infile);
      } /* end if */
      lexer->error_count++;
    } /* end if */
  } /* end while */
  
  return next_char;
} /* end get_pragma */


/* --------------------------------------------------------------------------
 * private function get_ident(lexer)
 * ----------------------------------------------------------------------- */

static char get_ident(m2c_lexer_t lexer) {
  
  char next_char;
  char next_next_char;
  
  m2c_mark_lexeme(lexer->infile);
  next_char = m2c_consume_char(lexer->infile);
  next_next_char = m2c_la2_char(lexer->infile);
  
  /* lowline enabled */
  if (m2c_option_lowline_identifiers()) {
    while (IS_ALPHANUMERIC(next_char) ||
           (next_char == '_' && IS_ALPHANUMERIC(next_next_char))) {
      next_char = m2c_consume_char(lexer->infile);
      next_next_char = m2c_la2_char(lexer->infile);
    } /* end while */
  }
  
  /* lowline disabled */
  else {
    while (IS_ALPHANUMERIC(next_char)) {
      next_char = m2c_consume_char(lexer->infile);
    } /* end while */
  } /* end if */
  
  /* get lexeme */
  lexer->lookahead.lexeme = m2c_read_marked_lexeme(lexer->infile);
    
  return next_char;
} /* end get_ident */


/* --------------------------------------------------------------------------
 * private function get_ident_or_resword(lexer)
 * ----------------------------------------------------------------------- */

#define IS_NOT_UPPER(c) \
  (((c) < 'A') || ((c) > 'Z'))

static char get_ident_or_resword(m2c_lexer_t lexer, m2c_token_t *token) {
  
  m2c_token_t intermediate_token;
  bool possibly_resword = true;
  char next_char;
  char next_next_char;
  
  m2c_mark_lexeme(lexer->infile);
  next_char = m2c_next_char(lexer->infile);
  next_next_char = m2c_la2_char(lexer->infile);
  
  /* lowline enabled */
  if (m2c_option_lowline_identifiers()) {
    while (IS_ALPHANUMERIC(next_char) ||
           (next_char == '_' && IS_ALPHANUMERIC(next_next_char))) {
      
      if (IS_NOT_UPPER(next_char)) {
        possibly_resword = false;
      } /* end if */
      
      next_char = m2c_consume_char(lexer->infile);
      next_next_char = m2c_la2_char(lexer->infile);
    } /* end while */
  }
  
  /* lowline disabled */
  else {
    while (IS_ALPHANUMERIC(next_char)) {
      
      if (IS_NOT_UPPER(next_char)) {
        possibly_resword = false;
      } /* end if */
      
      next_char = m2c_consume_char(lexer->infile);
    } /* end while */
  } /* end if */
  
  /* get lexeme */
  lexer->lookahead.lexeme = m2c_read_marked_lexeme(lexer->infile);
  
  /* check if lexeme is reserved word */
  if (possibly_resword) {
    intermediate_token =
      m2c_token_for_resword
        (m2c_string_char_ptr(lexer->lookahead.lexeme),
         m2c_string_length(lexer->lookahead.lexeme));
    if (intermediate_token != TOKEN_UNKNOWN) {
      *token = intermediate_token;
    }
    else {
      *token = TOKEN_IDENTIFIER;
    } /* end if */
  }
  else {
    *token = TOKEN_IDENTIFIER;
  } /* end if */
  
  return next_char;
} /* end get_ident_or_resword */

/* --------------------------------------------------------------------------
 * private function get_string_literal(lexer)
 * ----------------------------------------------------------------------- */

static char get_string_literal(m2c_lexer_t lexer, m2c_token_t *token) {
  
  uint_t line, column;
  m2c_token_t intermediate_token;
  char next_char, string_delimiter;
  
  intermediate_token = TOKEN_STRING;
  
  /* consume opening delimiter */
  string_delimiter = m2c_read_char(lexer->infile);
  
  m2c_mark_lexeme(lexer->infile);
  next_char = m2c_next_char(lexer->infile);
  
  while (next_char != string_delimiter) {
    
    /* check for control character */
    if (IS_CONTROL_CHAR(next_char)) {
      line = m2c_infile_current_line(lexer->infile);
      column = m2c_infile_current_column(lexer->infile);
      
      intermediate_token = TOKEN_MALFORMED_STRING;
      
      /* newline */
      if (next_char == '\n') {
        report_error_w_offending_pos
          (M2C_ERROR_NEW_LINE_IN_STRING_LITERAL, lexer,  line, column);
        break;
      }
      /* end-of-file marker */
      else if (IS_EOF(lexer, next_char)) {
        report_error_w_offending_pos
          (M2C_ERROR_EOF_IN_STRING_LITERAL, lexer,  line, column);
        break;
      }
      else /* any other control character */ {
        /* invalid input character */
        report_error_w_offending_char
          (M2C_ERROR_INVALID_INPUT_CHAR, lexer,  line, column, next_char);
      } /* end if */
    } /* end if */
    
    if (m2c_option_escape_tab_and_newline() && (next_char == '\\')) {
      line = m2c_infile_current_line(lexer->infile);
      column = m2c_infile_current_column(lexer->infile);
      next_char = m2c_consume_char(lexer->infile);
      
      if ((next_char != 'n') && (next_char != 't') && (next_char != '\\')) {
        /* invalid escape sequence */
        report_error_w_offending_char
          (M2C_ERROR_INVALID_ESCAPE_SEQUENCE,
           lexer,  line, column, next_char);
      } /* end if */
    } /* end if */
    
    next_char = m2c_consume_char(lexer->infile);
  } /* end while */
  
  /* get lexeme */
  lexer->lookahead.lexeme = m2c_read_marked_lexeme(lexer->infile);
  
  /* consume closing delimiter */
  if (next_char == string_delimiter) {
    next_char = m2c_consume_char(lexer->infile);
  } /* end if */
  
  /* pass back token */
  *token = intermediate_token;
  
  return next_char;
} /* end get_string_literal */


/* --------------------------------------------------------------------------
 * private function get_number_literal(lexer, token)
 * ----------------------------------------------------------------------- */

static char get_number_literal (m2c_lexer_t lexer, m2c_token_t *token) {
  
  m2c_token_t intermediate_token;
  char next_char, la2_char;
  
  m2c_mark_lexeme(lexer->infile);
  next_char = m2c_next_char(lexer->infile);
  la2_char = m2c_la2_char(lexer->infile);
  
  if /* prefix for base-16 integer or character code found */
    ((next_char == 0) && ((la2_char == 'x') || (la2_char == 'u'))) {

    /* consume '0' */
    next_char = m2c_consume_char(lexer->infile);
    
    if /* base-16 integer prefix */ (next_char == 'x') {
      intermediate_token = TOKEN_INTEGER;
    }
    else /* character code prefix */ {
      intermediate_token = TOKEN_CHAR;
    } /* end if */
   
    /* consume prefix */
    next_char = m2c_consume_char(lexer->infile);
    
    /* consume all digits */
    while (IS_DIGIT(next_char) || IS_A_TO_F(next_char)) {
      next_char = m2c_consume_char(lexer->infile);
    } /* end while */
  }
  else /* decimal integer or real number */ {
    
    /* consume all digits */
    while (IS_DIGIT(next_char)) {
      next_char = m2c_consume_char(lexer->infile);
    } /* end while */
    
    if /* real number literal found */ 
      ((next_char == '.') && (m2c_la2_char(lexer->infile) != '.')) {
      
      next_char =
        get_number_literal_fractional_part(lexer, &intermediate_token);
    }
    else {
      intermediate_token = TOKEN_INTEGER;
    } /* end if */
  } /* end if */
  
  /* get lexeme */
  lexer->lookahead.lexeme = m2c_read_marked_lexeme(lexer->infile);
  
  /* pass back token */
  *token = intermediate_token;
  
  return next_char;
} /* end get_number_literal */


/* --------------------------------------------------------------------------
 * private function get_number_literal_fractional_part(lexer, token)
 * ----------------------------------------------------------------------- */

static char get_number_literal_fractional_part
  (m2c_lexer_t lexer, m2c_token_t *token) {
  
  m2c_token_t intermediate_token = TOKEN_REAL;
  char next_char;
    
  /* consume the decimal point */
  next_char = m2c_consume_char(lexer->infile);
  
  /* consume any fractional digits */
  while (IS_DIGIT(next_char)) {
    next_char = m2c_consume_char(lexer->infile);
  } /* end if */
  
  if /* exponent prefix found */ (next_char == 'E') {
  
    /* consume exponent prefix */
    next_char = m2c_consume_char(lexer->infile);
    
    if /* exponent sign found*/
      ((next_char == '+') || (next_char == '-')) {
      
      /* consume exponent sign */
      next_char = m2c_consume_char(lexer->infile);
    } /* end if */
    
    if /* exponent digits found */ (IS_DIGIT(next_char)) {
    
      /* consume exponent digits */
      while (IS_DIGIT(next_char)) {
        next_char = m2c_consume_char(lexer->infile);
      } /* end while */
    }
    else /* exponent digits missing */ {
      intermediate_token = TOKEN_MALFORMED_REAL;
    } /* end if */
  } /* end if */
  
  /* pass back token */
  *token = intermediate_token;
  
  return next_char;
} /* end get_number_literal_fractional_part */