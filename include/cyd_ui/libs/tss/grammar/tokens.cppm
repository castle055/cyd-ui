/*! \file  tokens.cppm
 *! \brief
 *!
 */
module;
#include "bnf_macros.h"

export module cydui.styling.lang.parser:tokens;
export import :terminals;

import std;
export import reflect;
import reflect.serialize;
export import fabric.grammar;
import fabric.grammar.operators;

export import cydui.dimensions;
export import cydui.styling.lang.selectors;
export import cydui.styling.lang.rules;
export import cydui.graphics.types;


namespace syntax {
  export GRAMMAR(tss_tokens)
      : base_terminals {
    RULE(skip_wn)(*(whitespace | newline | return_));

    RULE(comma_separator)(
      *(whitespace | newline | return_), comma, *(whitespace | newline | return_)
    );
    RULE(colon_asigner)(
      *(whitespace | newline | return_), colon, *(whitespace | newline | return_)
    );
    RULE(braces_begin)(
      *(whitespace | newline | return_), left_braces, *(whitespace | newline | return_)
    );
    RULE(braces_end)(
      *(whitespace | newline | return_), right_braces, *(whitespace | newline | return_)
    );

    RULE(end_of_declaration)(
      *(whitespace | newline | return_), semicolon, *(whitespace | newline | return_)
    );

    TRULE(digit_excluding_zero)(d1 | d2 | d3 | d4 | d5 | d6 | d7 | d8 | d9);
    TRULE(digit)(d0 | digit_excluding_zero{});
    TRULE(hex_digit_chars)(a | b | c | d | e | f | A | B | C | D | E | F);
    TRULE(hex_digit)(hex_digit_chars{} | digit{});

    RULE(number)(~(plus | minus), ((dot, +digit{}) | (+digit{}, ~(dot, +digit{}))));

    TRULE(alpha)(
      a | b | c | d | e | f | g | h | i | j | k | l | m | n | o | p | q | r | s | t | u | v | w | x
      | y | z
    );
    TRULE(ALPHA)(
      A | B | C | D | E | F | G | H | I | J | K | L | M | N | O | P | Q | R | S | T | U | V | W | X
      | Y | Z
    );

    TRULE(alphanum)(alpha{} | ALPHA{} | digit{});

    TRULE(tss_identifier)(
      (alpha{} | ALPHA{} | minus | underscore), *(alphanum{} | minus | underscore)
    );

    TRULE(tss_single_quoted_str_char)(
      alphanum{} | colon | semicolon | plus | minus | underscore | shebang | dot | comma | asterisc
      | whitespace | newline | return_ | double_quote | left_brackets | right_brackets | left_braces
      | right_braces | left_parens | right_parens | left_angle_brackets | right_angle_brackets
      | forward_slash
    );
    TRULE(tss_single_quoted_str)(!quote, *tss_single_quoted_str_char{}, !quote);

    TRULE(tss_double_quoted_str_char)(
      alphanum{} | colon | semicolon | plus | minus | underscore | shebang | dot | comma | asterisc
      | whitespace | newline | return_ | quote | left_brackets | right_brackets | left_braces
      | right_braces | left_parens | right_parens | left_angle_brackets | right_angle_brackets
      | forward_slash
    );
    TRULE(tss_double_quoted_str)(!double_quote, *tss_double_quoted_str_char{}, !double_quote);
  };
} // namespace syntax
