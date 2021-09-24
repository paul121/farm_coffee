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

  // Add commands to add entities of each farm entity type.
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

    // Add a command linking to the add_form route for each bundle.
    foreach ($entity_manager->getStorage($bundle_entity_type)->loadMultiple() as $bundle_id => $bundle) {
      if ($entity_manager->getAccessControlHandler($entity_type_id)->createAccess($bundle_id)) {
        $commands[] = [
          'value'   => Url::fromRoute("entity.$entity_type_id.add_form", [$bundle_entity_type => $bundle_id])->toString(),
          'label'   => $bundle->label(),
          'command' => $command_prefix . ' ' . $bundle->label(),
        ];
      }
    }
  }

  return $commands;
}
