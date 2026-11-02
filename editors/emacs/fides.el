;;; fides.el --- Fides files with `fid lsp' through eglot (Emacs 29+)
(define-derived-mode fides-mode prog-mode "Fides"
  "Major mode for Fides (.fd) files."
  (setq-local comment-start "// ")
  (setq-local comment-end ""))
(add-to-list 'auto-mode-alist '("\.fd\'" . fides-mode))
(with-eval-after-load 'eglot
  (add-to-list 'eglot-server-programs '(fides-mode . ("fid" "lsp"))))
(add-hook 'fides-mode-hook #'eglot-ensure)
(provide 'fides)
;;; fides.el ends here
