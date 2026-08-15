(function(){
  // Parallax overlay for body::before using CSS variable --parallax-y
  if (typeof window === 'undefined') return;
  const mq = window.matchMedia('(prefers-reduced-motion: reduce)');
  const minWidth = 768;
  if (mq && mq.matches) return; // respect reduced motion
  if (window.innerWidth < minWidth) return; // disable on small screens

  let latestScroll = 0;
  let ticking = false;
  const maxOffset = 120; // px max translation
  function onScroll(){
    latestScroll = window.scrollY || window.pageYOffset || 0;
    requestTick();
  }
  function requestTick(){
    if (!ticking){
      requestAnimationFrame(update);
      ticking = true;
    }
  }
  function update(){
    ticking = false;
    // gentle parallax factor, slower than scroll
    const factor = 0.12; // adjust parallax strength
    let y = Math.round(latestScroll * factor);
    if (y > maxOffset) y = maxOffset;
    // invert for subtle upward movement
    document.documentElement.style.setProperty('--parallax-y', `-${y}px`);
  }
  window.addEventListener('scroll', onScroll, { passive: true });
  // initialize
  latestScroll = window.scrollY || window.pageYOffset || 0;
  requestTick();
  // update on resize to enable/disable
  window.addEventListener('resize', function(){
    if (window.innerWidth < minWidth){
      document.documentElement.style.setProperty('--parallax-y','0px');
    } else {
      latestScroll = window.scrollY || window.pageYOffset || 0; requestTick();
    }
  });
})();
