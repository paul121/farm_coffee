<?php

/**
 * @file
 * Coffee hooks implemented by the farm_coffee module.
 */

use Drupal\Core\Url;

/**
 * Implements hook_coffee_commands().
 */
function farm_coffee_coffee_commands() {
  $commands = [];
  $entity_manager = \Drupal::entityTypeManager();

  // Build commands specific to each farm entity type.
  $command_prefix = ':add';
  $farm_entity_types = ['asset', 'log', 'plan'];
  foreach ($farm_entity_types as $entity_type_id) {

    // Skip if the entity type does not exist.
    if (!$entity_manager->hasDefinition($entity_type_id)) {
      break;
    }

    // Get the bundle entity type.
    $entity_type = $entity_manager->getDefinition($entity_type_id);
    $bundle_entity_type = $entity_type->getBundleEntityType();

    // Build commands specific to each bundle.
    foreach ($entity_manager->getStorage($bundle_entity_type)->loadMultiple() as $bundle_id => $bundle) {

      // Add a command linking to the page_type view for each bundle.
      if ($url = Url::fromRoute("view.farm_$entity_type_id.page_type", ['arg_0' => $bundle_id])) {
        $commands[] = [
          'value' => $url->toString(),
          'label' => $bundle->label(),
          'command' => '',
        ];
      }

      // Add a command linking to the add_form route for each bundle.
      if ($entity_manager->getAccessControlHandler($entity_type_id)->createAccess($bundle_id)) {
        $commands[] = [
          'value'   => Url::fromRoute("entity.$entity_type_id.add_form", [$bundle_entity_type => $bundle_id])->toString(),
          'label'   => $bundle->label(),
          'command' => $command_prefix . ' ' . $bundle->label(),
        ];
      }
    }
  }

  /** @var \Drupal\Core\Menu\LocalTaskManagerInterface $local_task_manager */
  $local_task_manager = \Drupal::service('plugin.manager.menu.local_task');

  /** @var \Drupal\Core\Access\AccessManagerInterface $access_manager */
  $access_manager = \Drupal::service('access_manager');

  // Add commands for each farm_settings page.
  $settings_tree = $local_task_manager->getLocalTasksForRoute('farm_settings.settings_page');
  foreach ($settings_tree as $instances) {
    foreach ($instances as $child) {
      $route_name = $child->getRouteName();
      if ($access_manager->checkNamedRoute($route_name)) {
        $commands[] = [
          'value'   => Url::fromRoute($route_name)->toString(),
          'label'   => $child->getTitle(),
          'command' => ':settings' . ' ' . $child->getTitle(),
        ];
      }
    }
  }

  return $commands;
}
