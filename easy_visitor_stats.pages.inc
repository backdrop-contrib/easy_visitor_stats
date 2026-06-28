<?php
/**
 * @file
 * Page callbacks and related helper functions for Easy Visitor Statistics.
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
  // Crude display, almost raw data, but currently only used for debugging.
  // Maybe later implement Views integration.
  $header = array('href', 'duration', 'timezone', 'referrer', 'os', 'browser',
    'timestamp', 'ip');
  $data = db_query("SELECT * FROM {easy_visitor_details} ORDER BY timestamp DESC")->fetchAllAssoc('did');

  $page_result = array();
  if ($data) {
    $rows_per_page = 30;
    $current_page = pager_default_initialize(count($data), $rows_per_page);
    $chunks = array_chunk($data, $rows_per_page, TRUE);
    $page_result = $chunks[$current_page];
  }

  $rows = array();
  // Use today as base to avoid DST shifts.
  $today_midnight = strtotime('today midnight');
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
    '#empty' => t('No data yet.'),
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
  $form['#config'] = 'easy_visitor_stats.settings';
  $config = config('easy_visitor_stats.settings');
  $form['keep_detailed_days'] = array(
    '#type' => 'number',
    '#title' => t('How many days to keep detailed data'),
    '#min' => 7,
    '#max' => 60,
    '#field_suffix' => t('days'),
    '#default_value' => $config->get('keep_detailed_days'),
    '#required' => TRUE,
    '#description' => t('Database records older than that will be cleared out from <em>easy_visitor_details</em> table on cron runs.'),
  );
  $form['keep_history_months'] = array(
    '#type' => 'number',
    '#title' => t('How many months to keep history data'),
    '#min' => 3,
    '#max' => 36,
    '#field_suffix' => t('months'),
    '#default_value' => $config->get('keep_history_months'),
    '#required' => TRUE,
    '#description' => t('Database records older than that will be cleared out from <em>easy_visitor_history</em> table on cron runs.'),
  );
  $form['show_animations'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show animated charts'),
    '#default_value' => $config->get('show_animations'),
    '#description' => t('Animated charts can be nice, but might also be distracting.'),
  );
  $form['#submit'][] = '_easy_visitor_stats_check_site_key';
  return system_settings_form($form);
}

/**
 * Additional submission callback for settings form.
 */
function _easy_visitor_stats_check_site_key($form, $form_state) {
  // This key gets created on install, and even if it's missing it's not fatal,
  // so just to make sure there is one.
  $site_key = state_get('easyVisitorStatsSiteKey', FALSE);
  if (!$site_key) {
    $site_key = backdrop_random_key(16);
    state_set('easyVisitorStatsSiteKey', $site_key);
    // Because the easyVisitorStatsSiteKey JS setting is part of cached pages.
    cache('page')->flush();
    backdrop_set_message(t('A site key has been generated, as it was missing.'));
  }
}
