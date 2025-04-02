#lang racket
(begin 
  (displayln (list 1 2 3))
  (display "the car of (1 2 3) is ")
  (displayln (car (list 1 2 3)))
  (display "the cdr of (1 2 3) is ")
  (displayln (cdr (list 1 2 3)))
)
