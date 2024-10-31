#lang racket
(begin
 (define x 5)
 (display "x is ")
 (displayln x)
 (if (< x 7)
  (displayln "x is < 7")
  (displayln "x is >= 7"))
)
