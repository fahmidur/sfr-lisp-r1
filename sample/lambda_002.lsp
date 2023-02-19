#lang racket
(begin
  (define make_account
    (lambda ()
      (define balance 0)
      (lambda (amt)
        (begin
          (set! balance (+ balance amt))
          balance
        ))))
  (define account1 (make_account))
  (println account1)
  (displayln (account1 200))
  (displayln (account1 -20))
  (displayln (account1 -10))
  (displayln (account1 -5))
  (displayln (account1 40))
)
