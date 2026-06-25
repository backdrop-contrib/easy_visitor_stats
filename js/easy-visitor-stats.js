(function () {
  "use strict";

  // Initial value.
  document.addEventListener('DOMContentLoaded', function() {
    Backdrop.settings.easyVisitorStart = new Date().getTime();
  });

  document.addEventListener('visibilitychange', function() {
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
