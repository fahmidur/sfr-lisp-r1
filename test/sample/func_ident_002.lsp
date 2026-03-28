#lang racket
(begin
  (define ident (lambda (x) 
    (if (>= x 0)
      x
      (* x -1)
    )
  ))
  (displayln (ident -2))
  (displayln (ident -1))
  (displayln (ident 0))
  (displayln (ident 1))
  (displayln (ident 2))
)
