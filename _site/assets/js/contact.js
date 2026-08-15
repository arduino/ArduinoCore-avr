(function(){
  const form = document.getElementById('contact-form');
  if (!form) return;
  const status = document.getElementById('form-status');
  const submitBtn = form.querySelector('button[type="submit"]');
  const action = form.getAttribute('action');
  const PLACEHOLDER = '{your-form-id}';

  function showMessage(msg, ok=true){
    if (!status) return;
    status.textContent = msg;
    status.style.color = ok ? '' : '#b91c1c';
  }

  if (!action || action.indexOf(PLACEHOLDER) !== -1){
    showMessage('Form not configured — replace the action URL with your Formspree endpoint.', false);
    submitBtn.disabled = true;
    return;
  }

  form.addEventListener('submit', function(e){
    e.preventDefault();
    submitBtn.disabled = true;
    showMessage('Sending…', true);

    const data = new FormData(form);
    fetch(action, {
      method: 'POST',
      body: data,
      headers: { 'Accept': 'application/json' }
    }).then(function(response){
      if (response.ok){
        form.reset();
        showMessage('Thank you — your message has been sent.');
      } else {
        response.json().then(function(err){
          showMessage(err && err.error ? (err.error) : 'There was a problem sending the form.', false);
        }).catch(function(){
          showMessage('There was a problem sending the form.', false);
        });
      }
    }).catch(function(){
      showMessage('Network error — please try again later or email you@example.com', false);
    }).finally(function(){
      submitBtn.disabled = false;
    });
  });
})();
