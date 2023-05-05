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

