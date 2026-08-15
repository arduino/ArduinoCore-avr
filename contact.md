---
title: Contact
---

<section class="card">
  <h2>Get in touch</h2>
  <p class="kicker">Questions, feedback, or contributions — send a short message and I’ll reply.</p>

  <form id="contact-form" class="contact-form card" method="POST" action="https://formspree.io/f/xjybknwp">
    <label for="name">Name</label>
    <input id="name" name="name" type="text" required>

    <label for="email">Email</label>
    <input id="email" name="email" type="email" required>

    <label for="subject">Subject</label>
    <input id="subject" name="subject" type="text">

    <label for="message">Message</label>
    <textarea id="message" name="message" rows="6" required></textarea>

    <div class="form-actions">
      <button type="submit" class="btn">Send message</button>
    </div>

    <div id="form-status" role="status" aria-live="polite" style="margin-top:.5rem"></div>
  </form>

  <p>Or email: <a href="mailto:darianvergotine2204@proton.me">darianvergotine2204@proton.me</a></p>
  <p>GitHub: <a href="https://github.com/darianvergotine2026">darianvergotine2026</a></p>

  <h3>Contributing</h3>
  <p>Open an issue or pull request to suggest content or fixes. Include screenshots or sample files when relevant.</p>
</section>

<script src="/assets/js/contact.js" defer></script>
