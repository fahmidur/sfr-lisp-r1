#lang racket
(begin
 (define x 5)
 (display "x is ")
 (displayln x)
 (if (> x 8)
  (displayln "x is > 8")
  (displayln "x is <= 8"))
)
