<?php
/**
 * @file
 */

/**
 * Implements
 */
function template_preprocess_easy_visitor_stats_dashboard(&$variables) {
  // Get all data. @todo "order by" necessary?
  $data = db_query("SELECT * FROM {easy_visitor_details}")->fetchAllAssoc('did');
  $rows = array();
  foreach ($data as $values) {
    $values = (array) $values;
    unset($values['did']);
    $rows[] = $values;
  }
  $variables['total_page_hits'] = count($rows);

  $durations = array_filter(array_column($rows, 'duration'));
  $average = 0;
  if (count($durations)) {
    $average = array_sum($durations) / count($durations) / 1000;
  }
  $variables['average_time_spent'] = number_format($average, 2);

  // Hits based on hourly data.
  $date = new DateTime('now');
  if (count($rows)) {
    $first_timestamp = $rows[0]['timestamp'];
  }
  else {
    $first_timestamp = REQUEST_TIME - 60;
  }
  $site_timezone = new DateTimeZone(config_get('system.date','default_timezone'));
  $min_date = new DateTime('@' . $first_timestamp);
  $min_date->setTimezone($site_timezone);
  $options = array();
  while ($date > $min_date) {
    $key = $date->format('U');
    $label = format_date($date->format('U'), 'short');
    $date->modify('-1 week');
    $key = $date->format('U') . ',' . $key;
    $label = format_date($date->format('U'), 'short') . ' - ' . $label;
    $options[$key] = $label;
  }
  $toggle_select = array(
    '#type' => 'select',
    '#title' => t('Select range'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => '',
    '#attributes' => array(
      'id' => 'hits-graph-date-toggle',
    ),
  );
  $variables['hits_select'] = backdrop_render($toggle_select);
  $hits_chart_data = array(
    'labels' => array(),
    'datasets' => array(
      0 => array(
        'label' => 'Page hits',
        'data' => array(),
      )
    ),
  );

  $browser_stats = array_count_values(array_column($rows, 'browser'));
  arsort($browser_stats);
  $browser_chart_data = array(
    'labels' => array_keys($browser_stats),
    'datasets' => array(
      0 => array(
        'label' => 'Browsers',
        'data' => array_values($browser_stats),
      ),
    )
  );

  $os_stats = array_count_values(array_column($rows, 'os'));
  arsort($os_stats);
  $os_chart_data = array(
    'labels' => array_keys($os_stats),
    'datasets' => array(
      0 => array(
        'label' => 'OS',
        'data' => array_values($os_stats),
      ),
    )
  );

  $timezone_stats = array_count_values(array_column($rows, 'timezone'));
  arsort($timezone_stats);
  $timezone_chart_data = array(
    'labels' => array_keys($timezone_stats),
    'datasets' => array(
      0 => array(
        'label' => 'Timezone',
        'data' => array_values($timezone_stats),
      ),
    )
  );

  // More detailed duration data, reuses $durations from boxes above.
  $range_seconds = array();
  if (count($durations)) {
    $range_seconds = array(
      '0s - 15s' => 0,
      '15s - 60s' => 0,
      '60s - 300s' => 0,
      'longer' => 0,
    );
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

  // Simple line chart, granularity day. Actual data get fetched dynamically.
  $history_chart_data = array(
    'labels' => array(),
    'datasets' => array(
      0 => array(
        'label' => 'Page hits',
        'data' => array(),
      )
    ),
  );

  $date = new DateTime('now');
  $min_date_string = db_query("SELECT MIN(date) FROM {easy_visitor_history}")->fetchField();
  if (!$min_date_string) {
    $min_date_string = '1 minute ago';
  }
  $min_date = new DateTime($min_date_string);
  $options = array();
  while ($date > $min_date) {
    $key = $date->format('Y-m-d');
    $label = format_date($date->format('U'), 'date_only');
    $date->modify('-1 month');
    $key = $date->format('Y-m-d') . ',' . $key;
    $label = format_date($date->format('U'), 'date_only') . ' - ' . $label;
    $options[$key] = $label;
  }
  $toggle_select = array(
    '#type' => 'select',
    '#title' => t('Select range'),
    '#title_display' => 'invisible',
    '#options' => $options,
    '#default_value' => '',
    '#attributes' => array(
      'id' => 'history-graph-date-toggle',
    ),
  );
  $variables['history_select'] = backdrop_render($toggle_select);

  // Infotext for details element on top.
  $config = config('easy_visitor_stats.settings');
  $infotext = array();
  $infotext[] = t('Detailed data are available from @start_date until now. Detailed data include URL, referrer, duration, browser, OS and timezone info. These detailed data get dropped after @days days.', array(
    '@start_date' => format_date($first_timestamp),// Re-using variable from above.
    '@days' => $config->get('keep_detailed_days'),
  ));
  $infotext[] = t('History data are available from @start_date. History data only contain hit count per day. These data get dropped after @months months.', array(
    '@start_date' => format_date($min_date->format('U'), 'date_only'),
    '@months' => $config->get('keep_history_months'),
  ));
  $variables['infotext'] = implode('<br>', $infotext);

  // Collect all the charts data.
  backdrop_add_js(array(
    'easyVisitorStats' => array(
      'hitsData' => $hits_chart_data,
      'historyData' => $history_chart_data,
      'browserData' => $browser_chart_data,
      'osData' => $os_chart_data,
      'timezoneData' => $timezone_chart_data,
      'durationData' => $duration_chart_data,
      'historyDataFetchBase' => base_path() . 'easy-visitor-stats-fetch/history/',
      'hitsDataFetchBase' => base_path() . 'easy-visitor-stats-fetch/hits/',
      'showAnimation' => (bool) config_get('easy_visitor_stats.settings', 'show_animations'),
    ),
  ), 'setting');

  $top_ten = array();
  $top_ten['top_ten_wrapper'] = array(
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
    if (strlen($href) > 50) {
      $href_truncated = substr($href, 0, 50) . '…';
      $top_href_items[] = "<a href='$href' title='$href'>$href_truncated</a> ($count)";
    }
    else {
      $top_href_items[] = "<a href='$href'>$href</a> ($count)";
    }
  }
  $top_ten['top_ten_wrapper']['top_pages'] = array(
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
      $referrer_truncated = substr($referrer, 0, 50) . '…';
      $top_referrer_items[] = "<a target='_blank' href='$referrer' title='$referrer'>$referrer_truncated</a> ($count)";
    }
    else {
      $top_referrer_items[] = "<a target='blank' href='$referrer'>$referrer</a> ($count)";
    }
  }
  $top_ten['top_ten_wrapper']['top_referrer'] = array(
    '#theme' => 'item_list',
    '#items' => $top_referrer_items,
    '#type' => 'ol',
    '#title' => t('Top external referrers'),
  );
  $variables['top_ten'] = backdrop_render($top_ten);

  // @todo Idea: some sort of year overview (granulariy month).
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
