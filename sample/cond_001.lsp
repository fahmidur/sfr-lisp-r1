#lang racket
(begin
 (define x 5)
 (displayln "x is " x)
 (if (> x 8)
  (displayln "x is > 8")
  (displayln "x is <= 8"))
)
