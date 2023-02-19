#lang racket
(begin
  (define make_account
    (lambda (balance)
      (lambda (amt)
        (begin
          (set! balance (+ balance amt))
          balance
        ))))
  (define account1 (make_account 100))
  (displayln (account1 -20))
  (displayln (account1 -10))
  (displayln (account1 -5))
  (displayln (account1 40))
)
