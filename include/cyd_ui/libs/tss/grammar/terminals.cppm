/*! \file  terminals.cppm
 *! \brief 
 *!
 */
module;
#include "bnf_macros.h"

export module cydui.styling.lang.parser:terminals;

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
  export GRAMMAR(base_terminals) {
    TERMINAL_DIGIT(0);
    TERMINAL_DIGIT(1);
    TERMINAL_DIGIT(2);
    TERMINAL_DIGIT(3);
    TERMINAL_DIGIT(4);
    TERMINAL_DIGIT(5);
    TERMINAL_DIGIT(6);
    TERMINAL_DIGIT(7);
    TERMINAL_DIGIT(8);
    TERMINAL_DIGIT(9);

    TERMINAL(a);
    TERMINAL(A);
    TERMINAL(b);
    TERMINAL(B);
    TERMINAL(c);
    TERMINAL(C);
    TERMINAL(d);
    TERMINAL(D);
    TERMINAL(e);
    TERMINAL(E);
    TERMINAL(f);
    TERMINAL(F);
    TERMINAL(g);
    TERMINAL(G);
    TERMINAL(h);
    TERMINAL(H);
    TERMINAL(i);
    TERMINAL(I);
    TERMINAL(j);
    TERMINAL(J);
    TERMINAL(k);
    TERMINAL(K);
    TERMINAL(l);
    TERMINAL(L);
    TERMINAL(m);
    TERMINAL(M);
    TERMINAL(n);
    TERMINAL(N);
    TERMINAL(o);
    TERMINAL(O);
    TERMINAL(p);
    TERMINAL(P);
    TERMINAL(q);
    TERMINAL(Q);
    TERMINAL(r);
    TERMINAL(R);
    TERMINAL(s);
    TERMINAL(S);
    TERMINAL(t);
    TERMINAL(T);
    TERMINAL(u);
    TERMINAL(U);
    TERMINAL(v);
    TERMINAL(V);
    TERMINAL(w);
    TERMINAL(W);
    TERMINAL(x);
    TERMINAL(X);
    TERMINAL(y);
    TERMINAL(Y);
    TERMINAL(z);
    TERMINAL(Z);

    TERMINAL_W_NAME(':', colon);
    TERMINAL_W_NAME(';', semicolon);
    TERMINAL_W_NAME('+', plus);
    TERMINAL_W_NAME('-', minus);
    TERMINAL_W_NAME('_', underscore);
    TERMINAL_W_NAME('#', shebang);
    TERMINAL_W_NAME('.', dot);
    TERMINAL_W_NAME(',', comma);
    TERMINAL_W_NAME('*', asterisc);
    TERMINAL_W_NAME(' ', whitespace);
    TERMINAL_W_NAME('\n', newline);
    TERMINAL_W_NAME('\r', return_);
    TERMINAL_W_NAME('\'', quote);
    TERMINAL_W_NAME('"', double_quote);

    TERMINAL_W_NAME('[', left_brackets);
    TERMINAL_W_NAME(']', right_brackets);
    TERMINAL_W_NAME('{', left_braces);
    TERMINAL_W_NAME('}', right_braces);
    TERMINAL_W_NAME('(', left_parens);
    TERMINAL_W_NAME(')', right_parens);
    TERMINAL_W_NAME('<', left_angle_brackets);
    TERMINAL_W_NAME('>', right_angle_brackets);
    TERMINAL_W_NAME('/', forward_slash);
  };
}
