<?php
/**
 * Template for the dashboard.
 *
 * Available variables:
 * - $infotext
 * - $total_page_hits
 * - $average_time_spent
 * - $history_select
 * - $top_ten
 */
?>
<div class="easy-visitor-stats-dashboard">
  <div class="stats-info">
    <?php print $infotext; ?>
  </div>
  <div class="easy-visitor-boxes">
    <div class="box">
      <strong><?php print t('Total page hits'); ?></strong>
      <div><?php print $total_page_hits; ?></div>
    </div>
    <div class="box">
      <strong><?php print t('Average time spent on page'); ?></strong>
      <div><?php print $average_time_spent; ?>s</div>
    </div>
  </div>
  <h2><?php print t('Recent visits'); ?></h2>
  <div class="easy-visitor-hits-wrapper">
    <div class="chart-title"><?php print t('Page hits per hour'); ?></div>
    <canvas id="easy-visitor-hits-chart" width="1156" height="400" aria-label="@todo" role="img"></canvas>
  </div>
  <!-- Begin pie charts -->
  <h2><?php print t('About visitors'); ?></h2>
  <div class="easy-visitor-pie-wrapper">
    <div>
      <div class="chart-title"><?php print t('Browsers'); ?></div>
      <div class="pie-chart-wrapper">
        <canvas id="easy-visitor-browser-chart" width="150" height="150" aria-label="@todo" role="img"></canvas>
        <div id="easy-visitor-browser-legend" class="chart-legend"></div>
      </div>
    </div>
    <div>
      <div class="chart-title"><?php print t('Operating systems'); ?></div>
      <div class="pie-chart-wrapper">
        <canvas id="easy-visitor-os-chart" width="150" height="150" aria-label="@todo" role="img"></canvas>
        <div id="easy-visitor-os-legend" class="chart-legend"></div>
      </div>
    </div>
    <div>
      <div class="chart-title"><?php print t('Timezones'); ?></div>
      <div class="pie-chart-wrapper">
        <canvas id="easy-visitor-timezone-chart" width="150" height="150" aria-label="@todo" role="img"></canvas>
        <div id="easy-visitor-timezone-legend" class="chart-legend"></div>
      </div>
    </div>
    <div>
      <div class="chart-title"><?php print t('Time spent on page'); ?></div>
      <div class="pie-chart-wrapper">
        <canvas id="easy-visitor-duration-chart" width="150" height="150" aria-label="@todo" role="img"></canvas>
        <div id="easy-visitor-duration-legend" class="chart-legend"></div>
      </div>
    </div>
  </div>
  <!-- End pie charts -->
  <h2><?php print t('Visits history'); ?></h2>
  <div class="easy-visitor-history-wrapper">
    <div class="easy-visitor-history-toggle"><?php print $history_select; ?></div>
    <div class="chart-title"><?php print t('Page hits per day'); ?></div>
    <canvas id="easy-visitor-history-chart" width="1156" height="400" aria-label="@todo" role="img"></canvas>
  </div>
  <h2><?php print t('High score lists'); ?></h2>
  <div class="easy-visitor-top-ten-wrapper">
    <?php print $top_ten; ?>
  </div>
</div>
