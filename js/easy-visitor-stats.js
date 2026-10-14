(function () {
  "use strict";

  // Initial value.
  document.addEventListener('DOMContentLoaded', function() {
    Backdrop.settings.easyVisitorStart = new Date().getTime();
  });

  document.addEventListener('visibilitychange', function() {
    // Filter out automated clients early, as most of these checks can only
    // happen client side.
    if (navigator.userAgent.includes('HeadlessChrome') || navigator.webdriver === true) {
      return;
    }
    // Detect Playwright specifically.
    if (typeof window.exposed !== 'undefined' && typeof window.exposed.__installed !== 'undefined') {
      return;
    }
    if ('__playwright__binding__' in window || '__pwInitScripts' in window) {
      return;
    }
    // More like a plausibility check.
    if ((!navigator.languages || navigator.languages.length === 0) || !navigator.plugins.length) {
      return;
    }

    // Moved away from this browser tab, then comes back - reset timer. Moving
    // away already triggered sendBeacon().
    if (document.visibilityState === 'visible') {
      Backdrop.settings.easyVisitorStart = new Date().getTime();
    }

    // Move away or close tab or close browser.
    if (document.visibilityState === 'hidden') {
      if (!Backdrop.settings.easyVisitorStatsBeaconUrl) {
        return;
      }
      const fetchUrl = Backdrop.settings.easyVisitorStatsBeaconUrl;
      const siteKey = Backdrop.settings.easyVisitorStatsSiteKey;
      const now = new Date().getTime();
      let startTime = now;
      if (Backdrop.settings.easyVisitorStart !== undefined) {
        startTime = Backdrop.settings.easyVisitorStart;
      }
      const duration = (now - startTime);
      if (duration < 1000) {
        // Too short to consider.
        return;
      }
      const data = {
        href: window.location.href,
        duration: duration,
        timezone: Intl.DateTimeFormat().resolvedOptions().timeZone,
        referrer: document.referrer,
        platform: navigator.platform,
        ua: navigator.userAgent
      };
      // In order to let those show up in POST for PHP it has to be FormData.
      let formData = new FormData();
      for (const property in data) {
        formData.append(property, data[property]);
      }
      formData.append('siteKey', siteKey);
      navigator.sendBeacon(fetchUrl, formData);
    }
  });
})();
