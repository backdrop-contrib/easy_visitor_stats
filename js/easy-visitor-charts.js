(function ($) {
  "use strict";
  Backdrop.behaviors.easyVisitorCharts = {

    attach: function (context, settings) {
      const historyChartElement = document.getElementById('easy-visitor-history-chart');
      // Inherit color from parent. @todo find a better element to pick color.
      const styles = getComputedStyle(historyChartElement);
      Chart.defaults.color = styles.color;
      const historyChart = new Chart(historyChartElement, {
        type: 'line',
        options: {
          animation: false,
          scales: {
            y: {
              min: 0,
            }
          },
          plugins: {
            legend: {
              display: false,
            },
          },
        },
        data: settings.easyVisitorStats.historyData,
      });

      const pieChartOptions = {
        animation: false,
        responsive: false,
        maintainAspectRatio: false,
        radius: 70,
        plugins: {
          legend: {
            position: 'right',
            onClick: function () {},
          },
        }
      };
      // Browser pie chart.
      const browserChartElement = document.getElementById('easy-visitor-browser-chart');
      const browserChart = new Chart(browserChartElement, {
        type: 'doughnut',
        options: pieChartOptions,
        data: settings.easyVisitorStats.browserData,
      });

      // OS pie chart.
      const osChartElement = document.getElementById('easy-visitor-os-chart');
      const osChart = new Chart(osChartElement, {
        type: 'doughnut',
        options: pieChartOptions,
        data: settings.easyVisitorStats.osData,
      });

      // Timezone chart.
      const timezoneChartElement = document.getElementById('easy-visitor-timezone-chart');
      const timezoneChart = new Chart(timezoneChartElement, {
        type: 'doughnut',
        options: pieChartOptions,
        data: settings.easyVisitorStats.timezoneData,
      });

    }
  };
})(jQuery);
