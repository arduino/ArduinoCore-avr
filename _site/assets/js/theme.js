// Simple theme toggle (light/dark) persisted in localStorage
(function(){
  const root = document.documentElement;
  const toggle = document.getElementById('theme-toggle');
  const THEME_KEY = 'bracket:theme';

  function setTheme(theme){
    if(theme === 'dark'){
      root.classList.add('dark');
      root.classList.remove('light');
      document.body.classList.remove('theme-system');
      if(toggle) toggle.textContent = '☀️';
    } else if(theme === 'light'){
      root.classList.remove('dark');
      root.classList.add('light');
      document.body.classList.remove('theme-system');
      if(toggle) toggle.textContent = '🌙';
    } else {
      // system
      document.body.classList.add('theme-system');
      const prefersDark = window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches;
      setTheme(prefersDark ? 'dark' : 'light');
    }
  }

  // initialize
  try {
    const stored = localStorage.getItem(THEME_KEY);
    if(stored){ setTheme(stored); }
    else { setTheme('system'); }
  } catch(e){ setTheme('system'); }

  if(toggle){
    toggle.addEventListener('click', ()=>{
      const isDark = root.classList.contains('dark');
      const next = isDark ? 'light' : 'dark';
      try{ localStorage.setItem(THEME_KEY, next); } catch(e){}
      setTheme(next);
    });
  }
})();
