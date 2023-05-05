/* --------------------------------------------------------------------------
 * function collect_lowercase_word(src_index, source, tgt_index, target)
 * --------------------------------------------------------------------------
 * Matches the character sequence in source starting at src_index to a lower-
 * case word  and copies it to target starting at tgt_index.  Passes back the
 * positions  following  the last matched  and  the last copied  character in
 * src_index and tgt_index respectively.
 *
 * EBNF:  lowercaseWord := LowercaseLetter (LowercaseLetter | Digit)*;
 * ----------------------------------------------------------------------- */

static void collect_lowercase_word
  (uint_t *src_index, char *source, uint_t *tgt_index, char *target) {
  
  char ch;
  uint_t si, ti;

  si = *src_index;
  ch = source[si];
  
  /* first char must be lowercase */
  if (NOT(IS_LOWER(ch))) {
    return;
  }; /* end if */
  
  ti = *tgt_index;
  
  /* LowercaseLetter */
  target[ti] = ch;
  ti++;

  /* (LowercaseLetter | Digit)* */
  si++;
  ch = source[si];
  while ((IS_LOWER(ch) || IS_DIGIT(ch))) {
    target[ti] = ch;
    ti++;
    ch = source[si];
    si++;
  }; /* end while */
  
  /* pass indices back */
  *src_index = si;
  *tgt_index = ti;
  return;
}; /* end collect_lowercase_word */


/* --------------------------------------------------------------------------
 * function collect_titlecase_word(src_index, source, tgt_index, target)
 * --------------------------------------------------------------------------
 * Matches the character sequence in source starting at src_index to a title-
 * case word  and copies it to target starting at tgt_index.  Passes back the
 * positions  following  the last matched  and  the last copied  character in
 * src_index and tgt_index respectively.
 *
 * EBNF:  titlecaseWord := UppercaseLetter lowercaseWord;
 * ----------------------------------------------------------------------- */

static void collect_titlecase_word
  (uint_t *src_index, char *source, uint_t *tgt_index, char *target) {
  
  char ch;
  uint_t si, ti;
  
  si = *src_index;
  ch = source[si];
  
  /* first char must be uppercase */
  if (NOT(IS_UPPER(ch))) {
    return;
  }; /* end if */

  ti = *tgt_index;
  
  /* UppercaseLetter */
  target[ti] = ch;
  ti++;
  
  /* (LowercaseLetter | Digit)* */
  si++;
  ch = source[si];
  while ((IS_LOWER(ch) || IS_DIGIT(ch))) {
    target[ti] = ch;
    ti++;
    ch = source[si];
    si++;
  }; /* end while */
  
  /* pass indices back */
  *src_index = si;
  *tgt_index = ti;
  return;
}; /* end collect_titlecase_word */


/* --------------------------------------------------------------------------
 * function collect_uppercase_word(src_index, source, tgt_index, target)
 * --------------------------------------------------------------------------
 * Matches the character sequence in source starting at src_index to a title-
 * case word  and copies it to target starting at tgt_index.  Passes back the
 * positions  following  the last matched  and  the last copied  character in
 * src_index and tgt_index respectively.
 *
 * EBNF:  uppercaseWord := UppercaseLetter (UppercaseLetter | Digit)*;
 * ----------------------------------------------------------------------- */

static void collect_uppercase_word
  (uint_t *src_index, char *source, uint_t *tgt_index, char *target) {
  
  char ch;
  uint_t si, ti;
  
  si = *src_index;
  ch = source[si];
  
  /* first char must be uppercase */
  if (NOT(IS_UPPER(ch))) {
    return;
  }; /* end if */

  ti = *tgt_index;
  
  /* UppercaseLetter */
  /* (UppercaseLetter | Digit)* */
  while (((IS_UPPER(ch) && (IS_UPPER(source[si+1]))) || IS_DIGIT(ch))) {
    target[ti] = ch;
    ti++;
    ch = source[si];
    si++;
  }; /* end while */
  
  /* pass indices back */
  *src_index = si;
  *tgt_index = ti;
  return;
}; /* end collect_uppercase_word */



/* --------------------------------------------------------------------------
 * Highest possible number of words within an identifier
 * ----------------------------------------------------------------------- */

#define MAX_IDENT_WORDS ((M2C_MAX_IDENT_LENGTH / 2) + 1)


/* --------------------------------------------------------------------------
 * type word_entry_t
 * --------------------------------------------------------------------------
 * Length and position of a word within an identifier
 * ----------------------------------------------------------------------- */

typedef struct {
    uint8_t len;
    uint8_t pos;
} word_entry_t;


/* --------------------------------------------------------------------------
 * type word_map_t
 * --------------------------------------------------------------------------
 * Lengths and positions of all words within an identifier
 * ----------------------------------------------------------------------- */

typedef struct {
  uint8_t word_count;
  word_entry_t word[MAX_IDENT_WORDS];
} word_map_t;


/* --------------------------------------------------------------------------
 * function match_lowercase_word (index, ident)
 * --------------------------------------------------------------------------
 * Matches the character sequence in ident  starting at index  to a lowercase
 * word. Returns the length of the matched word or zero if there is no match.
 *
 * EBNF:  lowercaseWord := LowercaseLetter (LowercaseLetter | Digit)*;
 * ----------------------------------------------------------------------- */

static uint_t match_lowercase_word (uint_t index, const char *ident) {
  
  char ch;
  uint_t len;
  
  /* initial lookahead */
  ch = ident[index];
  len = 0;
  
  /* LowercaseLetter */
  if (IS_LOWER(ch)) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }
  else /* not lowercase */ {
    /* bail out */
    return 0;
  }; /* end if */
    
  /* (LowercaseLetter | Digit)* */
  while ((IS_LOWER(ch) || IS_DIGIT(ch))) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }; /* end while */
  
  /* matched word length */
  return len;
}; /* end match_lowercase_word */


/* --------------------------------------------------------------------------
 * function match_titlecase_word (index, ident)
 * --------------------------------------------------------------------------
 * Matches the character sequence in ident  starting at index  to a titlecase
 * word. Returns the length of the matched word or zero if there is no match.
 *
 * EBNF:  titlecaseWord := UppercaseLetter (LowercaseLetter | Digit)*;
 * ----------------------------------------------------------------------- */

static uint_t match_titlecase_word (uint_t index, const char *ident) {
  
  char ch;
  uint_t len;
  
  /* initial lookahead */
  ch = ident[index];
  len = 0;
  
  /* UppercaseLetter */
  if (IS_UPPER(ch)) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }
  else /* not uppercase */ {
    /* bail out */
    return 0;
  }; /* end if */
    
  /* (LowercaseLetter | Digit)* */
  while ((IS_LOWER(ch) || IS_DIGIT(ch))) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }; /* end while */
  
  /* matched word length */
  return len;
}; /* end match_titlecase_word */


/* --------------------------------------------------------------------------
 * function match_uppercase_word (index, ident)
 * --------------------------------------------------------------------------
 * Matches the character sequence in ident starting at index  to an uppercase
 * word. Returns the length of the matched word or zero if there is no match.
 *
 * EBNF:  uppercaseWord :=
 *          UppercaseLetter !LowercaseLetter
 *          (UppercaseLetter !LowercaseLetter | Digit)*;
 *
 * !!! An uppercase letter before a lowercase letter must NOT be consumed !!!
 * !!! because it is the first character of a following titlecase word.   !!!
 * ----------------------------------------------------------------------- */

static uint_t match_uppercase_word (uint_t index, const char *ident) {
  
  char ch;
  uint_t len;
  
  /* initial lookahead */
  ch = ident[index];
  len = 0;
  
  /* UppercaseLetter !LowercaseLetter */
  if (IS_UPPER(ch) && (NOT(IS_LOWER(ident[index+1])))) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }
  else /* not uppercase */ {
    /* bail out */
    return 0;
  }; /* end if */
    
  /* (UppercaseLetter !LowercaseLetter | Digit)* */
  while ((IS_UPPER(ch) && (NOT(IS_LOWER(ident[index+1])))) || IS_DIGIT(ch)) {
    /* consume lookahead */
    index++; len++;
    /* next lookahead */
    ch = ident[index];
  }; /* end while */
  
  /* matched word length */
  return len;
}; /* end match_uppercase_word */


/* --------------------------------------------------------------------------
 * function get_word_map_for_ident(ident, map)
 * --------------------------------------------------------------------------
 * Calculates a word map for ident, passes it in map  and returns word count.
 * If ident is malformed, passes zero filled map  and returns zero.
 * ----------------------------------------------------------------------- */

static void get_word_map_for_ident (const char *ident, word_map_t *map) {
  
  char ch;
  uint8_t pos, index;
  
  pos = 0;
  index = 0;
  ch = ident[pos];
  while (ch != ASCII_NUL) {
    /* lowercase word */
    if (IS_LOWER(ch)) {
      len = match_lowercase_word(pos, ident);
    }
    else if (IS_UPPER(ch)) {
      /* titlecase word */
      if (IS_LOWER(ident[pos+1])) {
        len = match_titlecase_word(pos, ident);
      }
      /* uppercase word */
      else {
        len = match_uppercase_word(pos, ident);
      }; /* end if */
    }
    /* lowline in identifier */
    else if (m2c_compiler_option_lowline_identifiers() && (ch == '_')) {
      /* skip lowline */
      pos++;
      continue;
    }
    /* no word match and no lowline */
    else /* malformed identifier */ {
      /* fill map with zeroes and zero word count */
      for (index = 0; index < MAX_IDENT_WORDS; index++) {
        map->word[index] = (word_entry_t) { 0, 0 };
      }; /* end for */
      map->word_count = 0;
      return;
    }; /* end if */
    
    /* word matched */
    if (len > 0) {
      /* pass length and position in map entry */
      map->word[index] = (word_entry_t) { len, pos };
      /* next word */
      pos = pos + len;
      /* next char */
      ch = ident[pos];
      /* next index */
      index++;
    }
    /* no match */
    else {
      break;
    }; /* end if */
  }; /* end while */
  
  /* pass word count */
  map->word_count = index;
  return;
}; /* end get_word_map_for_ident */


/* --------------------------------------------------------------------------
 * function required_length_for_snake_case(map)
 * --------------------------------------------------------------------------
 * Returns the required length for snake case translation from word map.
 * ----------------------------------------------------------------------- */

static uint8_t required_length_for_snake_case (const word_map_t *map) {
  
  uint8_t len, index, word_count;
  
  word_count = map->word_count;

  if (word_count == 0) {
    return 0;
  }; /* end if */
  
  /* add up all the word lengths */
  len = 0;
  index = 0;
  while (index < word_count) {
    len = len + map->word[index].len;
    index++;
  }; /* end while */
  
  /* add lowline count */
  len = len + word_count - 1;
  
  return len;
}; /* end required_length_for_snake_case */


static void copy_nth_word
  (const char *source, const word_map_t *map, char *target) {
  
  si = map->word[n].pos;
  len = map->word[n].len;

  while (si < len) {
    target[ti] = source[si];
    ti++; si++;
  }; /* end while */
  
  return;
}; /* end copy_nth_word */


static void copy_slice
  (const char *src, uint8_t srcpos, uint8_t len, char *tgt, uint8_t tgtpos) {
  
  uint8_t endpos;
  
  endpos = srcpos + len;
  while (srcpos < endpos) {
    tgt[tgtpos] = src[srcpos];
    tgtpos++; srcpos++;
  }; /* end while */

  return;
}; /* end copy_slice */


/* --------------------------------------------------------------------------
 * private type lettercase_t
 * --------------------------------------------------------------------------
 * Enumeration values for lower- and uppercase mode.
 * ----------------------------------------------------------------------- */

typedef enum {
    LC_LOWERCASE,
    LC_UPPERCASE
} lettercase_t;


/* --------------------------------------------------------------------------
 * private type case_transform_f
 * --------------------------------------------------------------------------
 * function pointer type for function to return case transformed character.
 * ----------------------------------------------------------------------- */

typedef char (case_transform_f) (char);


/* --------------------------------------------------------------------------
 * private function to_lower(ch)
 * --------------------------------------------------------------------------
 * If ch is uppercase, returns its lowercase counterpart, else returns ch.
 * ----------------------------------------------------------------------- */

static char to_lower(char ch) {
    if IS_UPPER(ch) {
      return ch + 32;
    }
    /* not uppercase */
    else {
      return ch;
    }; /* end if */
}; /* end to_lower */


/* --------------------------------------------------------------------------
 * private function to_upper(ch)
 * --------------------------------------------------------------------------
 * If ch is lowercase, returns its uppercase counterpart, else returns ch.
 * ----------------------------------------------------------------------- */

static char to_upper(char ch) {
    if IS_LOWER(ch) {
      return ch - 32;
    }
    /* not lowercase */
    else {
      return ch;
    }; /* end if */
}; /* end to_lower */


/* --------------------------------------------------------------------------
 * function lowline_transform(ident, map, lettercase, maxlen)
 * --------------------------------------------------------------------------
 * Returns the lowline and lettercase translation for ident using the given
 * word map.  If the result exceeds maxlen, it is truncated to maxlen.
 * ----------------------------------------------------------------------- */

static const char* lowline_transform
  (const char *ident, const word_map_t *map,
   lettercase_t lettercase, uint_t maxlen) {
  
  uint8_t word_count, xlat_len, index;
  case_transform_t case_transform;
  char *xlat;
  
  word_count = map->word_count;

  if (word_count == 0) {
    return NULL;
  }; /* end if */

  /* install case transformation handler */
  if (lettercase == LC_LOWERCASE) {
    case_transform = to_lower;
  }
  else /* LC_UPPERCASE */ {
    case_transform = to_upper;
  }; /* end if */
  
  xlat_len = required_length_for_snake_case(map);
  if xlat_len > maxlen {
    xlat_len = maxlen
  }; /* end if */
  xlat = malloc(xlat_len * sizeof(char) + 1);

  /* copy first word */
  tgt_index = 0;
  src_index = 0;
  len = map->word[0].len
  while ((src_index < len) && (tgt_index < maxlen)) {
    xlat[tgt_index] = case_transform(ident[src_index]);
    tgt_index++;
    src_index++;
  }; /* end while */
  
  word_index = 0;
  
  while (word_index < word_count) {
    /* append lowline */
    xlat[tgt_index] = '_';
    tgt_index++;
    
    /* copy word at index */
    len = map->word[word_index].len;
    src_index = map->word[word_index].pos;
    while ((src_index < len) && (tgt_index < maxlen)) {
      xlat[tgt_index] = case_transform(ident[src_index]);
      tgt_index++;
      src_index++;
    }; /* end while */
    
    /* next word */
    word_index++;
  }; /* end while */

  /* terminate string */
  xlat[tgt_index] = ASCII_NUL;
  
  return xlat;
}; /* end lowline_transform */