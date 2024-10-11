#lang racket
(begin
  (define make_account (lambda (balance) (lambda (amt) (begin (set! balance (+ balance amt)) balance ))))
  (define acc (make_account 100))
  (displayln (acc -20))
  (displayln (acc -10))
  (displayln (acc -5))
  (displayln (acc 40))
)
