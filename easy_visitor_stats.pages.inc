<?php
/**
 * @file
 */

/**
 * Page callback for admin/reports/easy-visitor-stats/dashboard.
 */
function _easy_visitor_stats_dashboard() {
  return theme('easy_visitor_stats_dashboard');
}

/**
 * Page callback for admin/reports/easy-visitor-stats/raw-data.
 */
function _easy_visitor_stats_raw() {
  $content = array();
  // extremely crude, needs pagination, needs responsive table classes, proper header...
  $header = array('href', 'duration', 'timezone', 'referrer', 'os', 'browser', 'timestamp', 'ip');
  $data = db_query("SELECT * FROM {easy_visitor_details} ORDER BY timestamp DESC")->fetchAllAssoc('did');

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
