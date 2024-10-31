#lang racket
(begin
 (define x 5)
 (display "x is ")
 (displayln x)
 (if (> x 2)
  (displayln "x is > 2")
  (displayln "x is <= 2"))
)
