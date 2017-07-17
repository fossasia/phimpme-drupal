<?php

namespace Drupal\ctools\Tests\Wizard;


use Drupal\simpletest\WebTestBase;
use Drupal\Core\StringTranslation\StringTranslationTrait;


/**
 * Tests basic wizard functionality.
 *
 * @group ctools
 */
class CToolsWizardTest extends WebTestBase {

  use StringTranslationTrait;
  public static $modules = array('ctools', 'ctools_wizard_test');

  function testWizardSteps() {
    $this->drupalGet('ctools/wizard');
    $this->assertText('Form One');
    $this->dumpHeaders = TRUE;
    // Check that $operations['one']['values'] worked.
    $this->assertText('Xylophone');
    // Submit first step in the wizard.
    $edit = [
      'one' => 'test',
    ];
    $this->drupalPostForm('ctools/wizard', $edit, $this->t('Next'));
    // Redirected to the second step.
    $this->assertText('Form Two');
    $this->assertText('Dynamic value submitted: Xylophone');
    // Check that $operations['two']['values'] worked.
    $this->assertText('Zebra');
    // Hit previous to make sure our form value are preserved.
    $this->drupalPostForm(NULL, [], $this->t('Previous'));
    // Check the known form values.
    $this->assertFieldByName('one', 'test');
    $this->assertText('Xylophone');
    // Goto next step again and finish this wizard.
    $this->drupalPostForm(NULL, [], $this->t('Next'));
    $edit = [
      'two' => 'Second test',
    ];
    $this->drupalPostForm(NULL, $edit, $this->t('Finish'));
    // Check that the wizard finished properly.
    $this->assertText('Value One: test');
    $this->assertText('Value Two: Second test');
  }

  function testStepValidateAndSubmit() {
    $this->drupalGet('ctools/wizard');
    $this->assertText('Form One');
    // Submit first step in the wizard.
    $edit = [
      'one' => 'wrong',
    ];
    $this->drupalPostForm('ctools/wizard', $edit, $this->t('Next'));
    // We're still on the first form and the error is present.
    $this->assertText('Form One');
    $this->assertText('Cannot set the value to "wrong".');
    // Try again with the magic value.
    $edit = [
      'one' => 'magic',
    ];
    $this->drupalPostForm('ctools/wizard', $edit, $this->t('Next'));
    // Redirected to the second step.
    $this->assertText('Form Two');
    $edit = [
      'two' => 'Second test',
    ];
    $this->drupalPostForm(NULL, $edit, $this->t('Finish'));
    // Check that the magic value triggered our submit callback.
    $this->assertText('Value One: Abraham');
    $this->assertText('Value Two: Second test');
  }

  function testEntityWizard() {
    $this->drupalLogin($this->drupalCreateUser(['administer site configuration']));

    // Start adding a new config entity.
    $this->drupalGet('admin/structure/ctools_wizard_test_config_entity/add');
    $this->assertText('Example entity');
    $this->assertNoText('Existing entity');

    // Submit the general step.
    $edit = [
      'id' => 'test123',
      'label' => 'Test Config Entity 123',
    ];
    $this->drupalPostForm(NULL, $edit, $this->t('Next'));

    // Submit the first step.
    $edit = [
      'one' => 'The first bit',
    ];
    $this->drupalPostForm(NULL, $edit, $this->t('Next'));

    // Submit the second step.
    $edit = [
      'two' => 'The second bit',
    ];
    $this->drupalPostForm(NULL, $edit, $this->t('Finish'));

    // Now we should be looking at the list of entities.
    $this->assertUrl('admin/structure/ctools_wizard_test_config_entity');
    $this->assertText('Test Config Entity 123');

    // Edit the entity again and make sure the values are what we expect.
    $this->clickLink(t('Edit'));
    $this->assertText('Existing entity');
    $this->assertFieldByName('label', 'Test Config Entity 123');
    $this->clickLink(t('Form One'));
    $this->assertFieldByName('one', 'The first bit');
    $previous = $this->getUrl();
    $this->clickLink(t('Show on dialog'));
    $this->assertRaw('Value from one: The first bit');
    $this->drupalGet($previous);
    // Change the value for 'one'.
    $this->drupalPostForm(NULL, ['one' => 'New value'], $this->t('Next'));
    $this->assertFieldByName('two', 'The second bit');
    $this->drupalPostForm(NULL, [], $this->t('Next'));
    // Make sure we get the additional step because the entity exists.
    $this->assertText('This step only shows if the entity is already existing!');
    $this->drupalPostForm(NULL, [], $this->t('Finish'));

    // Edit the entity again and make sure the change stuck.
    $this->assertUrl('admin/structure/ctools_wizard_test_config_entity');
    $this->clickLink(t('Edit'));
    $this->drupalPostForm(NULL, [], $this->t('Next'));
    $this->assertFieldByName('one', 'New value');
  }

}

