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

  // Simple line chart:
  // @todo WHERE in query... limit to date range? Where to display the full history?
  $history_data = db_query("SELECT * FROM {easy_visitor_history} ORDER BY date ASC")->fetchAllAssoc('date');;
  $line_chart_data = array(
    'labels' => array(),
    'datasets' => array(
      0 => array(
        'label' => 'Page hits',
        'data' => array(),
      )
    ),
  );
  foreach ($history_data as $date => $record) {
    $line_chart_data['labels'][] = $date;
    $line_chart_data['datasets'][0]['data'][] = $record->hits;
  }
  $content['history'] = array(
    '#markup' => '<strong>' . t('Page hits per day') . ':</strong><canvas id="easy-visitor-history-chart" class="easy-visitor-stats-chart" height="200" width="600"></canvas>',
  );

  $content['pie_wrapper'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array('easy-visitor-pie-wrapper'),
    ),
  );
  // Only most commom browsers are collected.
  $content['pie_wrapper']['browsers'] = array(
    '#markup' => '<canvas id="easy-visitor-browser-chart" class="easy-visitor-stats-chart" height="160" width="290"></canvas>',
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
    '#markup' => '<canvas id="easy-visitor-os-chart" class="easy-visitor-stats-chart" height="160" width="290"></canvas>',
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
    '#markup' => '<canvas id="easy-visitor-timezone-chart" class="easy-visitor-stats-chart" height="160" width="290"></canvas>',
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

  $module_path = backdrop_get_path('module', 'easy_visitor_stats');
  backdrop_add_js($module_path . '/libraries/chart.umd.min.js');
  backdrop_add_js($module_path . '/js/easy-visitor-charts.js');
  backdrop_add_css($module_path . '/css/easy-visitor-stats-chart.css');

  // Collect all the charts data.
  backdrop_add_js(array(
    'easyVisitorStats' => array(
      'historyData' => $line_chart_data,
      'browserData' => $browser_chart_data,
      'osData' => $os_chart_data,
      'timezoneData' => $timezone_chart_data,
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

  // Top referrers. @todo only external referrers make sense here!, custom filter callback
  $referrers = array_filter(array_column($rows, 'referrer'));
  $referrer_stats = array_count_values($referrers);
  arsort($referrer_stats);
  $top_referrer = array_slice($referrer_stats, 0, 10);
  $top_referrer_items = array();
  foreach ($top_referrer as $referrer => $count) {
    if (strlen($referrer) > 60) {
      $referrer_truncated = substr($referrer, 0, 59) . '…';
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
    '#title' => t('Top referrers'),
  );

  return $content;
}

/**
 * Page callback for admin/reports/easy-visitor-stats/raw-data.
 */
function _easy_visitor_stats_raw() {
  $content = array();
  // extremely crude, needs pagination, needs responsive table classes, proper header...
  $header = array('href', 'duration', 'timezone', 'referrer', 'os', 'browser', 'timestamp', 'ip');
  $data = db_query("SELECT * FROM {easy_visitor_details} ORDER BY timestamp DESC LIMIT 30")->fetchAllAssoc('did');
  $rows = array();
  $today_midnight = strtotime('today midnight');// must use today to avoid weird DST shifts
  foreach ($data as $values) {
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
  return $content;
}

/**
 * Form callback for admin/reports/easy-visitor-stats/settings.
 */
function _easy_visitor_stats_settings_form($form, $form_state) {
  $form = array();
  return $form;
}
