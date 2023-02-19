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
  (println (account1 -20))
  (println (account1 -10))
  (println (account1 -5))
  (println (account1 40))
)
