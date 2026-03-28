#lang racket
(begin
  (define ident (lambda (x) 
    x
  ))
  (displayln (ident 1))
  (displayln (ident 2))
  (displayln (ident 3))
  (displayln (ident 4))
)

