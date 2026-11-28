# External roadmap

Goals that no code change can finish: each one needs an outside party, money, or
people using Fides. They were moved here on Nov 21 2026 from the other roadmaps,
which keep a one-line `[>]` pointer where each goal was, so their IDs and
cross-references still work. None of these blocks a release.

## Security review

- [ ] **Independent security review** of the compiler and the runtime before
      anything is called production ready (from release J9, competitive "Set
      aside"). The internal evidence index is prepared at
      `docs/security/review-index.md`; the review itself needs an outside
      party: not done and not claimed.
  - [ ] External security audit of the compiler (from roadmap.md L10G).
  - [ ] Audit region safety: can it be bypassed? Our own evidence: competitive
        VF11, release M.
  - [ ] Audit capability security: can unprivileged code reach IO/FS/Net?
  - [ ] Audit the effect system: can effects be hidden?
  - [ ] Audit FFI: can unsafe C code bypass Fides safety? `fid audit` lists what
        a program reaches (release M10).
  - [ ] Publish the audit report.
- [ ] **A bug bounty** (from competitive "Set aside").

## Certification

- [ ] **A qualified toolchain process** per IEC 62304, ISO 26262 or DO-178C,
      through a certification body (from release L5, competitive "Set aside";
      XL, external).

## Production use

- [ ] **AD5. A real production service of ours**, on Fides, with its incidents
      written up (from competitive AD5; L).
- [ ] **AD16. A first company running Fides in production** (from competitive
      AD16; XL, not only effort).
  - [ ] Find 3 companies willing to pilot Fides in production (from roadmap.md
        L10H).
  - [ ] Document use case, results, lessons learned.
  - [ ] Publish the case studies.
  - [ ] Exit criterion (roadmap.md L10): at least 3 production case studies.

## Third-party publishing

- [x] **N3. Publish the VS Code extension on the Marketplace** (from
      release-roadmap N3). Done Nov 28 2026: fid-lang 0.1.0 is live on the
      Visual Studio Marketplace
      (marketplace.visualstudio.com/items?itemName=pledgeandgrow.fid-lang,
      publisher pledgeandgrow) and on Open VSX
      (open-vsx.org/extension/pledgeandgrow/fid-lang, the registry VS Code
      forks — VSCodium, Cursor, Windsurf — actually query; the top hit for
      "fid" there on day one). The extension in editors/vscode/ was brought
      up to the language first: the grammar learned raw strings, float
      exponents and the ~25 keywords added since it was written; the dead
      fides-lang.org links became fid-lang.org; snippets, a changelog, the
      brand icon (docs/fid-logo.svg) and an error dialog that points at the
      install docs were added. Left as polish, not part of this goal: the
      Open VSX namespace verification checkmark (the claim issue for
      pledgeandgrow is filed with Eclipse) and the editor-setup page on the
      website, which still says the extension is not published.

## Community and adoption

- [ ] **AD14. Ten external projects** built on Fides (from competitive AD14; XL,
      not only effort).
- [ ] **AD15. Ten contributors** outside the core (from competitive AD15; XL,
      not only effort).
- [ ] **AD12. A public launch** once the beta gate is met, with honest limits
      (from competitive AD12; M).
- [ ] **A community forum** (Discourse or similar) and **Discord/Matrix** for
      real-time chat (from roadmap.md L10F). GitHub Discussions exists
      (competitive AD9).
  - [ ] Exit criterion (roadmap.md L10): an active community, forum and chat.

## Teaching and the book

- [ ] **University course** (from roadmap.md L10I):
  - [ ] Course materials at university level.
  - [ ] Topics: systems programming, memory safety, capability security, effect
        systems.
  - [ ] Lab exercises, using the playground.
  - [ ] Partner with at least one university to teach it (also the L10 exit
        criterion).
- [ ] **The book in print**: free online, paid print and ebook (from
      roadmap.md L10B). The book itself, `docs/book/`, is code-side work
      (competitive AD6).

## Organisation

- [ ] **A foundation, full-time staff, conferences** (from competitive "Set
      aside").

## Infrastructure

- [ ] Dedicated benchmark and fuzzing machines (from competitive "Set aside").
      Provisioning and funding require an external owner; the repository can
      prepare repeatable workloads, but cannot supply dedicated hardware.
