<?php

namespace Drupal\ctools_views\Tests;

use Drupal\views_ui\Tests\UITestBase;
use Drupal\views\Tests\ViewTestData;
use Drupal\Core\StringTranslation\StringTranslationTrait;

/**
 * Tests the ctools_views block display plugin
 * overriding settings from a basic View.
 *
 * @group ctools_views
 * @see \Drupal\ctools_views\Plugin\Display\Block
 */
class CToolsViewsBasicViewBlockTest extends UITestBase {

  use StringTranslationTrait;

  /**
   * Modules to enable.
   *
   * @var array
   */
  public static $modules = array('ctools_views', 'ctools_views_test_views');

  /**
   * Views used by this test.
   *
   * @var array
   */
  public static $testViews = array('ctools_views_test_view');

  /**
   * The block storage.
   *
   * @var \Drupal\Core\Entity\EntityStorageInterface
   */
  protected $storage;

  /**
   * @inheritdoc
   */
  protected function setUp() {
    parent::setUp();

    ViewTestData::createTestViews(get_class($this), array('ctools_views_test_views'));
    $this->storage = $this->container->get('entity.manager')->getStorage('block');
  }

  /**
   * Test ctools_views "items_per_page" configuration.
   */
  public function testItemsPerPage() {
    $default_theme = $this->config('system.theme')->get('default');

    // Get the "Configure block" form for our Views block.
    $this->drupalGet('admin/structure/block/add/views_block:ctools_views_test_view-block_pager/' . $default_theme);
    $this->assertFieldByXPath('//input[@type="number" and @name="settings[override][items_per_page]"]', NULL, 'items_per_page setting is a number field');
    // Add block to sidebar_first region with default settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][items_per_page]'] = 0;
    $this->drupalPostForm('admin/structure/block/add/views_block:ctools_views_test_view-block_pager/' . $default_theme, $edit, $this->t('Save block'));

    // Assert items per page default settings.
    $this->drupalGet('<front>');
    $result = $this->xpath('//div[contains(@class, "region-sidebar-first")]/div[contains(@class, "block-views")]/h2');
    $this->assertEqual((string) $result[0], 'CTools Views Pager Block');
    $this->assertRaw('Showing 3 records on page 1');
    $this->assertEqual(3, count($this->xpath('//div[contains(@class, "view-display-id-block_pager")]//table/tbody/tr')));

    // Override items per page settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][items_per_page]'] = 2;
    $this->drupalPostForm('admin/structure/block/manage/views_block__ctools_views_test_view_block_pager', $edit, $this->t('Save block'));

    $block = $this->storage->load('views_block__ctools_views_test_view_block_pager');
    $config = $block->getPlugin()->getConfiguration();
    $this->assertEqual(2, $config['items_per_page'], "'Items per page' is properly saved.");

    // Assert items per page overridden settings.
    $this->drupalGet('<front>');
    $result = $this->xpath('//div[contains(@class, "region-sidebar-first")]/div[contains(@class, "block-views")]/h2');
    $this->assertEqual((string) $result[0], 'CTools Views Pager Block');
    $this->assertRaw('Showing 2 records on page 1');
    $this->assertEqual(2, count($this->xpath('//div[contains(@class, "view-display-id-block_pager")]//table/tbody/tr')));
    $this->assertEqual([1, 2], $this->xpath('//div[contains(@class, "view-display-id-block_pager")]//table//tr//td[contains(@class, "views-field-id")]'));
  }

  /**
   * Test ctools_views "offset" configuration.
   */
  public function testOffset() {
    $default_theme = $this->config('system.theme')->get('default');

    // Get the "Configure block" form for our Views block.
    $this->drupalGet('admin/structure/block/add/views_block:ctools_views_test_view-block_pager/' . $default_theme);
    $this->assertFieldByXPath('//input[@type="number" and @name="settings[override][pager_offset]"]', NULL, 'items_per_page setting is a number field');
    // Add block to sidebar_first region with default settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][items_per_page]'] = 0;
    $this->drupalPostForm('admin/structure/block/add/views_block:ctools_views_test_view-block_pager/' . $default_theme, $edit, $this->t('Save block'));

    // Assert pager offset default settings.
    $this->drupalGet('<front>');
    $this->assertEqual([1, 2, 3], $this->xpath('//div[contains(@class, "view-display-id-block_pager")]//table//tr//td[contains(@class, "views-field-id")]'));

    // Override pager offset settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][items_per_page]'] = 0;
    $edit['settings[override][pager_offset]'] = 1;
    $this->drupalPostForm('admin/structure/block/manage/views_block__ctools_views_test_view_block_pager', $edit, $this->t('Save block'));

    $block = $this->storage->load('views_block__ctools_views_test_view_block_pager');
    $config = $block->getPlugin()->getConfiguration();
    $this->assertEqual(1, $config['pager_offset'], "'Pager offset' is properly saved.");

    // Assert pager offset overridden settings.
    $this->drupalGet('<front>');
    $this->assertEqual([2, 3, 4], $this->xpath('//div[contains(@class, "view-display-id-block_pager")]//table//tr//td[contains(@class, "views-field-id")]'));
  }

  /**
   * Test ctools_views "pager" configuration.
   */
  public function testPager() {
    $default_theme = $this->config('system.theme')->get('default');

    // Get the "Configure block" form for our Views block.
    $this->drupalGet('admin/structure/block/add/views_block:ctools_views_test_view-block_pager/' . $default_theme);
    $this->assertFieldById('edit-settings-override-pager-view', 'view');
    $this->assertFieldById('edit-settings-override-pager-some');
    $this->assertFieldById('edit-settings-override-pager-none');

    // Add block to sidebar_first region with default settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][items_per_page]'] = 0;
    $this->drupalPostForm('admin/structure/block/add/views_block:ctools_views_test_view-block_pager/' . $default_theme, $edit, $this->t('Save block'));

    // Assert pager default settings.
    $this->drupalGet('<front>');
    $this->assertText('Page 1');
    $this->assertText('Next ›');

    // Override pager settings to 'some'.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][items_per_page]'] = 0;
    $edit['settings[override][pager]'] = 'some';
    $this->drupalPostForm('admin/structure/block/manage/views_block__ctools_views_test_view_block_pager', $edit, $this->t('Save block'));

    $block = $this->storage->load('views_block__ctools_views_test_view_block_pager');
    $config = $block->getPlugin()->getConfiguration();
    $this->assertEqual('some', $config['pager'], "'Pager' setting is properly saved.");

    // Assert pager overridden settings to 'some', showing no pager.
    $this->drupalGet('<front>');
    $this->assertEqual(3, count($this->xpath('//div[contains(@class, "view-display-id-block_pager")]//table/tbody/tr')));
    $this->assertNoText('Page 1');
    $this->assertNoText('Next ›');

    // Override pager settings to 'none'.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][items_per_page]'] = 0;
    $edit['settings[override][pager]'] = 'none';
    $this->drupalPostForm('admin/structure/block/manage/views_block__ctools_views_test_view_block_pager', $edit, $this->t('Save block'));

    $block = $this->storage->load('views_block__ctools_views_test_view_block_pager');
    $config = $block->getPlugin()->getConfiguration();
    $this->assertEqual('none', $config['pager'], "'Pager' setting is properly saved.");

    // Assert pager overridden settings to 'some', showing no pager.
    $this->drupalGet('<front>');
    $this->assertEqual(5, count($this->xpath('//div[contains(@class, "view-display-id-block_pager")]//table/tbody/tr')));
    $this->assertNoText('Page 1');
    $this->assertNoText('Next ›');
  }

  /**
   * Test ctools_views 'hide_fields' configuration.
   */
  public function testHideFields() {
    $default_theme = $this->config('system.theme')->get('default');

    // Get the "Configure block" form for our Views block.
    $this->drupalGet('admin/structure/block/add/views_block:ctools_views_test_view-block_fields/' . $default_theme);
    $this->assertFieldById('edit-settings-override-order-fields-id-hide');

    // Add block to sidebar_first region with default settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $this->drupalPostForm('admin/structure/block/add/views_block:ctools_views_test_view-block_fields/' . $default_theme, $edit, $this->t('Save block'));

    // Assert hide_fields default settings.
    $this->drupalGet('<front>');
    $this->assertEqual(5, count($this->xpath('//div[contains(@class, "view-display-id-block_fields")]//table//td[contains(@class, "views-field-id")]')));

    // Override hide_fields settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][order_fields][id][hide]'] = 1;
    $this->drupalPostForm('admin/structure/block/manage/views_block__ctools_views_test_view_block_fields', $edit, $this->t('Save block'));

    $block = $this->storage->load('views_block__ctools_views_test_view_block_fields');
    $config = $block->getPlugin()->getConfiguration();
    $this->assertEqual(1, $config['fields']['id']['hide'], "'hide_fields' setting is properly saved.");
    $this->assertEqual(0, $config['fields']['name']['hide'], "'hide_fields' setting is properly saved.");

    // Assert hide_fields overridden settings.
    $this->drupalGet('<front>');
    $this->assertEqual(0, count($this->xpath('//div[contains(@class, "view-display-id-block_fields")]//table//td[contains(@class, "views-field-id")]')));
  }

  /**
   * Test ctools_views 'sort_fields' configuration.
   */
  public function testOrderFields() {
    $default_theme = $this->config('system.theme')->get('default');

    // Get the "Configure block" form for our Views block.
    $this->drupalGet('admin/structure/block/add/views_block:ctools_views_test_view-block_fields/' . $default_theme);
    $this->assertFieldById('edit-settings-override-order-fields-id-weight', 0);

    // Add block to sidebar_first region with default settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $this->drupalPostForm('admin/structure/block/add/views_block:ctools_views_test_view-block_fields/' . $default_theme, $edit, $this->t('Save block'));

    // Assert sort_fields default settings.
    $this->drupalGet('<front>');
    // Check that the td with class "views-field-id" is the first td in the first tr element.
    $this->assertEqual(0, count($this->xpath('count(//div[contains(@class, "view-display-id-block_fields")]//table//tr[1]//td[contains(@class, "views-field-id")]/preceding-sibling::td)')));

    // Override sort_fields settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][order_fields][name][weight]'] = -50;
    $edit['settings[override][order_fields][age][weight]'] = -49;
    $edit['settings[override][order_fields][job][weight]'] = -48;
    $edit['settings[override][order_fields][created][weight]'] = -47;
    $edit['settings[override][order_fields][id][weight]'] = -46;
    $edit['settings[override][order_fields][name_1][weight]'] = -45;
    $this->drupalPostForm('admin/structure/block/manage/views_block__ctools_views_test_view_block_fields', $edit, $this->t('Save block'));

    $block = $this->storage->load('views_block__ctools_views_test_view_block_fields');
    $config = $block->getPlugin()->getConfiguration();
    $this->assertEqual(-46, $config['fields']['id']['weight'], "'sort_fields' setting is properly saved.");
    $this->assertEqual(-50, $config['fields']['name']['weight'], "'sort_fields' setting is properly saved.");

    // Assert sort_fields overridden settings.
    $this->drupalGet('<front>');

    // Check that the td with class "views-field-id" is the 5th td in the first tr element.
    $this->assertEqual(4, count($this->xpath('//div[contains(@class, "view-display-id-block_fields")]//table//tr[1]//td[contains(@class, "views-field-id")]/preceding-sibling::td')));

    // Check that duplicate fields in the View produce expected outpu
    $name1_element = $this->xpath('//div[contains(@class, "view-display-id-block_fields")]//table//tr[1]/td[contains(@class, "views-field-name")]/text()');
    $name1 = (string) $name1_element[0];
    $this->assertEqual("John", trim($name1));
    $name2_element = $this->xpath('//div[contains(@class, "view-display-id-block_fields")]//table//tr[1]/td[contains(@class, "views-field-name-1")]/text()');
    $name2 = (string) $name2_element[0];
    $this->assertEqual("John", trim($name2));
  }

  /**
   * Test ctools_views 'disable_filters' configuration.
   */
  public function testDisableFilters() {
    $default_theme = $this->config('system.theme')->get('default');

    // Get the "Configure block" form for our Views block.
    $this->drupalGet('admin/structure/block/add/views_block:ctools_views_test_view-block_filter/' . $default_theme);
    $this->assertFieldById('edit-settings-override-filters-status-disable');
    $this->assertFieldById('edit-settings-override-filters-job-disable');

    // Add block to sidebar_first region with default settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $this->drupalPostForm('admin/structure/block/add/views_block:ctools_views_test_view-block_filter/' . $default_theme, $edit, $this->t('Save block'));

    // Assert disable_filters default settings.
    $this->drupalGet('<front>');
    // Check that the default settings show both filters
    $this->assertFieldByXPath('//select[@name="status"]');
    $this->assertFieldByXPath('//input[@name="job"]');

    // Override disable_filters settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][filters][status][disable]'] = 1;
    $edit['settings[override][filters][job][disable]'] = 1;
    $this->drupalPostForm('admin/structure/block/manage/views_block__ctools_views_test_view_block_filter', $edit, $this->t('Save block'));

    $block = $this->storage->load('views_block__ctools_views_test_view_block_filter');
    $config = $block->getPlugin()->getConfiguration();
    $this->assertEqual(1, $config['filter']['status']['disable'], "'disable_filters' setting is properly saved.");
    $this->assertEqual(1, $config['filter']['job']['disable'], "'disable_filters' setting is properly saved.");

    // Assert disable_filters overridden settings.
    $this->drupalGet('<front>');
    $this->assertNoFieldByXPath('//select[@name="status"]');
    $this->assertNoFieldByXPath('//input[@name="job"]');
  }

  /**
   * Test ctools_views 'configure_sorts' configuration.
   */
  public function testConfigureSorts() {
    $default_theme = $this->config('system.theme')->get('default');

    // Get the "Configure block" form for our Views block.
    $this->drupalGet('admin/structure/block/add/views_block:ctools_views_test_view-block_sort/' . $default_theme);
    $this->assertFieldByXPath('//input[@name="settings[override][sort][id][order]"]');

    // Add block to sidebar_first region with default settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $this->drupalPostForm('admin/structure/block/add/views_block:ctools_views_test_view-block_sort/' . $default_theme, $edit, $this->t('Save block'));

    // Assert configure_sorts default settings.
    $this->drupalGet('<front>');
    // Check that the results are sorted ASC
    $element = $this->xpath('//div[contains(@class, "view-display-id-block_sort")]//table//tr[1]/td[1]/text()');
    $value = (string) $element[0];
    $this->assertEqual("1", trim($value));

    // Override configure_sorts settings.
    $edit = array();
    $edit['region'] = 'sidebar_first';
    $edit['settings[override][sort][id][order]'] = "DESC";
    $this->drupalPostForm('admin/structure/block/manage/views_block__ctools_views_test_view_block_sort', $edit, $this->t('Save block'));

    $block = $this->storage->load('views_block__ctools_views_test_view_block_sort');
    $config = $block->getPlugin()->getConfiguration();
    $this->assertEqual("DESC", $config['sort']['id'], "'configure_sorts' setting is properly saved.");

    // Assert configure_sorts overridden settings.
    // Check that the results are sorted DESC
    $this->drupalGet('<front>');
    $element = $this->xpath('//div[contains(@class, "view-display-id-block_sort")]//table//tr[1]/td[1]/text()');
    $value = (string) $element[0];
    $this->assertEqual("5", trim($value));
  }
}
