#lang racket
(begin
 (define x 5)
 (display "x is ")
 (displayln x)
 (if (<= x 5)
  (displayln "x is <= 5")
  (displayln "x is > 5"))
)
