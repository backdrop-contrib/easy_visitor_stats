/**
 * @file
 * Draw various charts on the reports dashboard.
 */
(function ($) {
  "use strict";
  Backdrop.behaviors.easyVisitorCharts = {
    /**
     * Dynamically fetch charts data.
     *
     * @param object chartObj
     * @param string fetchUrl
     * @param string title
     */
    updateChart: function (chartObj, fetchUrl, title) {
      fetch(fetchUrl)
        .then(function(response) {
          return response.json();
        })
        .then(function(response) {
          chartObj.data.labels = response.labels;
          chartObj.data.datasets[0].data = response.values;
          chartObj.options.plugins.title.text = title;
          // Add some padding above.
          chartObj.options.scales.y.suggestedMax = Math.max(...response.values) + 1;
          chartObj.update();
        });
    },
    /**
     * {@inheritdoc}
     */
    attach: function (context, settings) {
      // Inherit color from content for better readability.
      const styles = getComputedStyle(document.querySelector('.easy-visitor-boxes'));
      Chart.defaults.color = styles.color;
      Chart.defaults.scale.ticks.autoSkipPadding = 15;
      Chart.defaults.elements.point.hitRadius = 10;
      Chart.defaults.elements.line.tension = 0.2;

      const hitsChartElement = document.getElementById('easy-visitor-hits-chart');
      const hitsData = settings.easyVisitorStats.hitsData;
      const hitsChart = new Chart(hitsChartElement, {
        type: 'line',
        options: {
          animation: false,
          scales: {
            y: {
              min: 0,
              suggestedMax: Math.max(...hitsData.datasets[0].data) + 1,
            },
          },
          plugins: {
            legend: {
              display: false,
            },
          },
          elements: {
            line: {
              fill: 'origin',
            },
          },
        },
        data: hitsData,
      });

      const historyChartElement = document.getElementById('easy-visitor-history-chart');
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
            title: {
              display: true,
              text: '',
            }
          },
        },
        data: settings.easyVisitorStats.historyData,
      });

      // Fetch dynamically.
      const fetchUrlBase = settings.easyVisitorStats.historyDataFetchBase;
      const toggleElement = document.getElementById('history-graph-date-toggle');
      const initialValues = toggleElement.value.split(',');
      let fetchUrl = fetchUrlBase + initialValues[0] + '/' + initialValues[1];
      let title = toggleElement.options[toggleElement.selectedIndex].text;
      Backdrop.behaviors.easyVisitorCharts.updateChart(historyChart, fetchUrl, title);

      toggleElement.addEventListener('change', function (event) {
        let newTitle = event.target.options[event.target.selectedIndex].text;
        let newValues = event.target.value.split(',');
        let newFetchUrl = fetchUrlBase + newValues[0] + '/' + newValues[1];
        Backdrop.behaviors.easyVisitorCharts.updateChart(historyChart, newFetchUrl, newTitle);
      });

      const pieChartOptions = {
        animation: false,
        responsive: false,
        maintainAspectRatio: false,
        radius: 64,
        plugins: {
          legend: {
            position: 'bottom',
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

      // Duration chart.
      const durationChartElement = document.getElementById('easy-visitor-duration-chart');
      const durationChart = new Chart(durationChartElement, {
        type: 'doughnut',
        options: pieChartOptions,
        data: settings.easyVisitorStats.durationData,
      });

    }
  };
})(jQuery);
