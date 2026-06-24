<?php
/**
 * @file
 */

/**
 * Page callback for admin/reports/easy-visitor-stats/dashboard.
 */
function _easy_visitor_stats_dashboard() {
  $content = array();
  $js_settings = array();

  // No LIMIT - possibly not needed here? Depends on settings?
  $data = db_query("SELECT * FROM {easy_visitor_details}")->fetchAllAssoc('did');
  $rows = array();
  foreach ($data as $values) {
    $values = (array) $values;
    unset($values['did']);
    $rows[] = $values;
  }
  // @todo clean up this messy function

  // Some explanations re time span.
  $content['intro'] = array(
    '#markup' => 'Detailed data are available from @start_date until now. Detailed data include URL, referrer, duration, browser, OS and timezone info. These detailed data get dropped after 7 days.',
  );
  // Topmost boxes with some numbers.
  $content['boxes'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('easy-visitor-boxes'),
    ),
  );
  $content['boxes']['total_hits'] = array(
    '#markup' => '<div class="box"><strong>' . t('Total page hits') . '</strong><div>' . count($rows) . '</div></div>',
  );
  $durations = array_filter(array_column($rows, 'duration'));
  $average = 0;
  if (count($durations)) {
    $average = array_sum($durations) / count($durations) / 1000;
  }
  $content['boxes']['average_duration'] = array(
    '#markup' => '<div class="box"><strong>' . t('Average time spent on page') . '</strong><div>' . number_format($average, 2) . 's</div></div>',
  );

  // Another line chart - hits based on hourly data.
  // @todo both line charts might better get data via AJAX.
  $timestamps = array_column($rows, 'timestamp');
  // Couldn't get Luxon working, so...
  //$begin = new DateTime('@' . reset($timestamps));
  $begin = new DateTime('@' . strtotime('1 week ago'));
  //$end = new DateTime('@' . end($timestamps));
  $end = new dateTime('@' . strtotime('now'));
  $hour_ranges = array();
  while($begin < $end) {
    $begin->modify('+1 hour');
    $key = $begin->format('Y-m-d H');
    $hour_ranges[$key] = 0;
  }
  $hits = array();
  foreach ($timestamps as $timestamp) {
    $date = format_date($timestamp, 'custom', 'Y-m-d H');
    if (array_key_exists($date, $hits)) {
      $hits[$date] = $hits[$date] + 1;
    }
    else {
      $hits[$date] = 1;
    }
  }
  $hits_range = array_merge($hour_ranges, $hits);
  $hits_chart_data = array(
    'labels' => array(),
    'datasets' => array(
      0 => array(
        'label' => 'Page hits',
        'data' => array(),
      )
    ),
  );
  $hits_chart_data['labels'] = array_keys($hits_range);
  $hits_chart_data['datasets'][0]['data'] = array_values($hits_range);
  $content['hits'] = array(
    '#markup' => '<strong>' . t('Page hits per hour') . ':</strong><canvas id="easy-visitor-hits-chart" class="easy-visitor-stats-chart" height="200" width="600"></canvas>',
  );

  $content['pie_wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('easy-visitor-pie-wrapper'),
    ),
  );

  // Only most commom browsers are collected.
  $content['pie_wrapper']['browsers'] = array(
    '#markup' => '<div>' . t('Browsers') . '<canvas id="easy-visitor-browser-chart" class="easy-visitor-stats-chart" height="220" width="320"></canvas></div>',
  );
  $browser_chart_data = array(
    'labels' => array(),
    'datasets' => array(
      0 => array(
        'label' => 'Browsers',
        'data' => array(),
      ),
    )
  );
  $browser_stats = array_count_values(array_column($rows, 'browser'));
  $browser_chart_data['labels'] = array_keys($browser_stats);
  $browser_chart_data['datasets'][0]['data'] = array_values($browser_stats);

  // Operating systems.
  $content['pie_wrapper']['os'] = array(
    '#markup' => '<div>' . t('Operating Systems') . '<canvas id="easy-visitor-os-chart" class="easy-visitor-stats-chart" height="220" width="320"></canvas></div>',
  );
  $os_chart_data = array(
    'labels' => array(),
    'datasets' => array(
      0 => array(
        'label' => 'OS',
        'data' => array(),
      ),
    )
  );
  $os_stats = array_count_values(array_column($rows, 'os'));
  $os_chart_data['labels'] = array_keys($os_stats);
  $os_chart_data['datasets'][0]['data'] = array_values($os_stats);

  // Timezones.
  $content['pie_wrapper']['timezone'] = array(
    '#markup' => '<div>' . t('Timezones') . '<canvas id="easy-visitor-timezone-chart" class="easy-visitor-stats-chart" height="220" width="320"></canvas></div>',
  );
  $timezone_chart_data = array(
    'labels' => array(),
    'datasets' => array(
      0 => array(
        'label' => 'Timezone',
        'data' => array(),
      ),
    )
  );
  $timezone_stats = array_count_values(array_column($rows, 'timezone'));
  $timezone_chart_data['labels'] = array_keys($timezone_stats);
  $timezone_chart_data['datasets'][0]['data'] = array_values($timezone_stats);
  // More detailed duration data, reuses $durations from boxes above.
  $range_seconds = array(
    '0s - 15s' => 0,
    '15s - 60s' => 0,
    '60s - 300s' => 0,
    'longer' => 0,
  );
  $ranges = array();
  if (count($durations)) {
    foreach ($durations as $duration_ms) {
      $duration = $duration_ms / 1000;
      if ($duration > 300) {
        $range_seconds['longer'] = $range_seconds['longer'] + 1;
      }
      elseif ($duration > 60) {
        $range_seconds['60s - 300s'] = $range_seconds['60s - 300s'] + 1;
      }
      elseif ($duration > 15) {
        $range_seconds['15s - 60s'] = $range_seconds['15s - 60s'] + 1;
      }
      else {
        $range_seconds['0s - 15s'] = $range_seconds['0s - 15s'] + 1;
      }
    }
  }
  $duration_chart_data = array(
    'labels' => array_keys($range_seconds),
    'datasets' => array(
      0 => array(
        'label' => 'Time spent on page',
        'data' => array_values($range_seconds),
      ),
    ),
  );
  $content['pie_wrapper']['time_spent'] = array(
    '#markup' => '<div>' . t('Time spent on page') . '<canvas id="easy-visitor-duration-chart" class="easy-visitor-stats-chart" height="220" width="320"></canvas></div>',
  );

  // Simple line chart, granularity day.
  // @todo this has to be ajax, with WHERE and all. Too many data points.
  $history_data = db_query("SELECT * FROM {easy_visitor_history} ORDER BY date ASC")->fetchAllAssoc('date');;
  $begin = new DateTime('@' . strtotime('1 month ago'));
  $end = new dateTime('@' . strtotime('now'));// reuse from above?
  $day_ranges = array($begin->format('Y-m-d') => 0);
  while($begin < $end) {
    $begin->modify('+1 day');
    $key = $begin->format('Y-m-d');
    $day_ranges[$key] = 0;
  }
  $content['history_info'] = array(
    '#markup' => '<p>The page hits history is available from @start_date until now. History data get dropped after @timespan.</p>',
  );
  $history_hits = array();
  foreach ($history_data as $date => $record) {
    $history_hits[$date] = $record->hits;
  }
  $line_chart_data = array(
    'labels' => array_keys($day_ranges),
    'datasets' => array(
      0 => array(
        'label' => 'Page hits',
        'data' => array_merge($day_ranges, $history_hits),
      )
    ),
  );
  $content['history'] = array(
    '#markup' => '<strong>' . t('Page hits per day') . ':</strong><canvas id="easy-visitor-history-chart" class="easy-visitor-stats-chart" height="200" width="600"></canvas>',
  );

  $module_path = backdrop_get_path('module', 'easy_visitor_stats');
  backdrop_add_js($module_path . '/libraries/chart.umd.min.js');
  backdrop_add_js($module_path . '/js/easy-visitor-charts.js');
  backdrop_add_css($module_path . '/css/easy-visitor-stats-chart.css');

  // Collect all the charts data.
  backdrop_add_js(array(
    'easyVisitorStats' => array(
      'hitsData' => $hits_chart_data,
      'historyData' => $line_chart_data,
      'browserData' => $browser_chart_data,
      'osData' => $os_chart_data,
      'timezoneData' => $timezone_chart_data,
      'durationData' => $duration_chart_data,
    ),
  ), 'setting');

  // Top pages.
  $content['top_ten_wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('easy-visitor-top-ten'),
    ),
  );
  $href_stats = array_count_values(array_column($rows, 'href'));
  arsort($href_stats);
  $top_href = array_slice($href_stats, 0, 10);
  $top_href_items = array();
  foreach ($top_href as $href => $count) {
    if (strlen($href) > 40) {
      $href_truncated = substr($href, 0, 39) . '…';
      $top_href_items[] = "<a href='$href' title='$href'>$href_truncated</a> ($count)";
    }
    else {
      $top_href_items[] = "<a href='$href'>$href</a> ($count)";
    }
  }
  $content['top_ten_wrapper']['top_pages'] = array(
    '#theme' => 'item_list',
    '#items' => $top_href_items,
    '#type' => 'ol',
    '#title' => t('Top pages'),
  );

  // Top external referrers.
  $referrers = _easy_visitor_stats_filter_referrers(array_column($rows, 'referrer'));
  $referrer_stats = array_count_values($referrers);
  arsort($referrer_stats);
  $top_referrer = array_slice($referrer_stats, 0, 10);
  $top_referrer_items = array();
  foreach ($top_referrer as $referrer => $count) {
    if (strlen($referrer) > 50) {
      $referrer_truncated = substr($referrer, 0, 49) . '…';
      $top_referrer_items[] = "<a target='_blank' href='$referrer' title='$referrer'>$referrer_truncated</a> ($count)";
    }
    else {
      $top_referrer_items[] = "<a target='blank' href='$referrer'>$referrer</a> ($count)";
    }
  }
  $content['top_ten_wrapper']['top_referrer'] = array(
    '#theme' => 'item_list',
    '#items' => $top_referrer_items,
    '#type' => 'ol',
    '#title' => t('Top external referrers'),
  );

  return $content;
}

/**
 * Helper function to filter out empty or referrers from this site.
 */
function _easy_visitor_stats_filter_referrers(array $referrer_data) {
  global $base_url;
  $filtered_referrers = array();
  foreach ($referrer_data as $referrer) {
    if (empty($referrer) || strpos($referrer, $base_url) === 0) {
      continue;
    }
    $filtered_referrers[] = $referrer;
  }
  return $filtered_referrers;
}

/**
 * Page callback for admin/reports/easy-visitor-stats/raw-data.
 */
function _easy_visitor_stats_raw() {
  $content = array();
  // extremely crude, needs pagination, needs responsive table classes, proper header...
  $header = array('href', 'duration', 'timezone', 'referrer', 'os', 'browser', 'timestamp', 'ip');
  $data = db_query("SELECT * FROM {easy_visitor_details} ORDER BY timestamp DESC LIMIT 30")->fetchAllAssoc('did');

  $page_result = array();
  if ($data) {
    $rows_per_page = 30;
    $current_page = pager_default_initialize(count($data), $rows_per_page);
    $chunks = array_chunk($data, $rows_per_page, TRUE);
    $page_result = $chunks[$current_page];
  }

  $rows = array();
  $today_midnight = strtotime('today midnight');// must use today to avoid weird DST shifts
  foreach ($page_result as $values) {
    $values = (array) $values;
    unset($values['did']);
    $values['timestamp'] = format_date($values['timestamp']);
    $ts = $today_midnight + ($values['duration'] / 1000);
    $values['duration'] = format_date($ts, 'custom', 'i:s.v');
    $rows[] = $values;
  }
  $content['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('No data yet.')
  );
  $content['pager'] = array(
    '#markup' => theme('pager', array(
      'quantity' => 7,
      count($page_result),
    )),
  );
  return $content;
}

/**
 * Form callback for admin/reports/easy-visitor-stats/settings.
 */
function _easy_visitor_stats_settings_form($form, $form_state) {
  $form = array();
  return $form;
}
