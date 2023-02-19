(begin
  (define make_account
    (lambda (balance)
      (lambda (amt)
        (begin
          (set! balance (+ balance amt))
          (balance)))))
  (define account1 (make_account 100))
  (print "account1=" account1)
  (print (account1 -20))
  (print (account1 -10))
  (print (account1 -5))
  (print (account1 40))
)
