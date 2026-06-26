/**
 * @file
 * Draw various charts on the reports dashboard.
 */
(function ($) {
  "use strict";

  /**
   * Custom chart.js label plugin.
   */
  const htmlLegendPlugin = {
    id: 'htmlLegend',
    afterUpdate(chart, args, options) {
      const items = chart.options.plugins.legend.labels.generateLabels(chart);
      const legendContainer = document.getElementById(options.containerID);
      if (!legendContainer) {
        return;
      }
      for (const item of items) {
        const div = document.createElement('div');
        const boxSpan = document.createElement('span');
        boxSpan.style.background = item.fillStyle;
        boxSpan.classList.add('legend-color-box');
        const text = document.createTextNode(item.text);
        div.appendChild(boxSpan);
        div.appendChild(text);
        legendContainer.appendChild(div);
      }
    }
  }

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
      const showAnimation = settings.easyVisitorStats.showAnimation;
      // Inherit color from content for better readability.
      const styles = getComputedStyle(document.querySelector('.easy-visitor-boxes'));
      Chart.defaults.color = styles.color;
      Chart.defaults.scale.ticks.autoSkipPadding = 15;
      Chart.defaults.elements.point.hitRadius = 10;
      Chart.defaults.elements.line.tension = 0.2;

      const hitsChartElement = document.getElementById('easy-visitor-hits-chart');
      const hitsData = settings.easyVisitorStats.hitsData;
      const hitsChartOptions = {
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
      };
      if (!showAnimation) {
        hitsChartOptions.animation = false;
      }
      const hitsChart = new Chart(hitsChartElement, {
        type: 'line',
        options: hitsChartOptions,
        data: hitsData,
      });

      const historyChartElement = document.getElementById('easy-visitor-history-chart');
      const historyChartOptions = {
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
      };
      if (!showAnimation) {
        historyChartOptions.animation = false;
      }
      const historyChart = new Chart(historyChartElement, {
        type: 'line',
        options: historyChartOptions,
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
        responsive: false,
        maintainAspectRatio: false,
        radius: 70,
        plugins: {
          legend: {
            display: false,
          },
          htmlLegend: {
            containerID: '',
          },
        },
      };
      if (!showAnimation) {
        pieChartOptions.animation = false;
      }
      // Browser pie chart.
      const browserChartElement = document.getElementById('easy-visitor-browser-chart');
      pieChartOptions.plugins.htmlLegend.containerID = 'easy-visitor-browser-legend';
      const browserChart = new Chart(browserChartElement, {
        type: 'doughnut',
        options: pieChartOptions,
        data: settings.easyVisitorStats.browserData,
        plugins: [htmlLegendPlugin],
      });

      // OS pie chart.
      const osChartElement = document.getElementById('easy-visitor-os-chart');
      pieChartOptions.plugins.htmlLegend.containerID = 'easy-visitor-os-legend';
      const osChart = new Chart(osChartElement, {
        type: 'doughnut',
        options: pieChartOptions,
        data: settings.easyVisitorStats.osData,
        plugins: [htmlLegendPlugin],
      });

      // Timezone chart.
      const timezoneChartElement = document.getElementById('easy-visitor-timezone-chart');
      pieChartOptions.plugins.htmlLegend.containerID = 'easy-visitor-timezone-legend';
      const timezoneChart = new Chart(timezoneChartElement, {
        type: 'doughnut',
        options: pieChartOptions,
        data: settings.easyVisitorStats.timezoneData,
        plugins: [htmlLegendPlugin],
      });

      // Duration chart.
      const durationChartElement = document.getElementById('easy-visitor-duration-chart');
      pieChartOptions.plugins.htmlLegend.containerID = 'easy-visitor-duration-legend';
      const durationChart = new Chart(durationChartElement, {
        type: 'doughnut',
        options: pieChartOptions,
        data: settings.easyVisitorStats.durationData,
        plugins: [htmlLegendPlugin],
      });

    }
  };
})(jQuery);
