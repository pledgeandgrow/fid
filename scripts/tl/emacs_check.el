;;; emacs_check.el --- editors/emacs/fides.el exercised in batch Emacs (TL10)
;;
;; Run by scripts/tl/editor-check.sh:
;;   emacs --batch -Q -l scripts/tl/emacs_check.el <config> <file.fd>
;; `fid' must be on PATH.  Checks, one "ok"/"FAIL" line each: .fd files open
;; in fides-mode, eglot knows `fid lsp' for it, fides-mode-hook starts
;; eglot, the server connects and publishes diagnostics for the file's
;; error.  Exit status 1 on any FAIL.

(require 'cl-lib)
(defvar ec-fails 0)
(defun ec-check (ok what &optional detail)
  (if ok
      (princ (format "ok   emacs: %s\n" what))
    (setq ec-fails (1+ ec-fails))
    (princ (format "FAIL emacs: %s%s\n" what (if detail (format " (%s)" detail) "")))))

(let* ((config (nth 0 command-line-args-left))
       (file (nth 1 command-line-args-left)))
  (setq command-line-args-left nil)
  (load (expand-file-name config) nil t)
  (require 'eglot)
  (ec-check (eq (assoc-default "dir/x.fd" auto-mode-alist #'string-match) 'fides-mode)
            "auto-mode-alist maps x.fd to fides-mode"
            (format "%S" (assoc-default "dir/x.fd" auto-mode-alist #'string-match)))
  (ec-check (not (eq (assoc-default "dir/x.fdx" auto-mode-alist #'string-match) 'fides-mode))
            "auto-mode-alist leaves x.fdx alone")
  (ec-check (equal (alist-get 'fides-mode eglot-server-programs) '("fid" "lsp"))
            "eglot-server-programs runs `fid lsp' for fides-mode"
            (format "%S" (alist-get 'fides-mode eglot-server-programs)))
  (ec-check (memq #'eglot-ensure fides-mode-hook) "fides-mode-hook starts eglot")
  ;; eglot-ensure waits for a command loop that batch Emacs does not run:
  ;; start the server the way it would
  (find-file file)
  (ec-check (eq major-mode 'fides-mode) "the file opens in fides-mode" (format "%S" major-mode))
  (let ((server (condition-case err
                    (progn (apply #'eglot (eglot--guess-contact)) (eglot-current-server))
                  (error (ec-check nil "eglot starts the server" (error-message-string err)) nil))))
    (when server
      (ec-check (jsonrpc-running-p server) "server fid connected")
      (let ((deadline (+ (float-time) 30)))
        (while (and (null (flymake-diagnostics)) (< (float-time) deadline))
          (when (fboundp 'flymake-start) (ignore-errors (flymake-start)))
          (accept-process-output nil 0.5)))
      (ec-check (flymake-diagnostics) "diagnostics published for the file's error")
      (when (flymake-diagnostics)
        (let ((d (car (flymake-diagnostics))))
          (princ (format "     first diagnostic: line %d: %s\n"
                         (line-number-at-pos (flymake-diagnostic-beg d))
                         (car (split-string (flymake-diagnostic-text d) "\n"))))))
      (ignore-errors (eglot-shutdown server)))))
(kill-emacs (if (= ec-fails 0) 0 1))
;;; emacs_check.el ends here
