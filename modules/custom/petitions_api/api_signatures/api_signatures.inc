<?php

/**
 * @file
 * Helper functions for the Signatures Resource module.
 */

/**
 * Adds a new signature to a petition.
 *
 * @param string $api_key
 *   The API key.
 * @param array $signature
 *   The signature argument received from the service request.
 */
function _api_signatures_resource_create($api_key, array $signature) {

  // Check for the required key.
  if (empty($api_key) || !api_key_manager_valid_key($api_key)) {
    $status_code = 401;
    $developer_message = t("Invalid key.");
    $user_message = t("The key provided is not valid.");
    return api_errors_throw_error($status_code, $developer_message, $user_message);
  }

  // Add the API Key to the signature.
  $signature['signature_source_api_key'] = $api_key;

  // Submit the signature.
  $options = array('signature' => $signature);
  $status_code = signatures_queue_invoke_workflow('receive_new_signatures', $options);

  // Deliver response.
  switch ($status_code) {
    case SIGNATURES_QUEUE_STATUS_OK:
      // Return successsful, generic response.
      return api_services_responses_format();

    case SIGNATURES_QUEUE_STATUS_BAD_REQUEST:
      $developer_message = $user_message = t('Missing required fields. These fields are required: signature_source_api_key, petition_id, email, first_name, last_name.');
      $more_info = 'https://github.com/WhiteHouse/petitions/issues/75';
      $error_code = 75;
      return api_errors_throw_error($status_code, $developer_message, $user_message, $more_info, $error_code);

    case SIGNATURES_QUEUE_STATUS_FORBIDDEN:
      global $base_url;
      $developer_message = t("Petition @petition_id cannot accept more signatures. (If you believe this petition was not open for a full 30 days, it's possible that it was flagged and removed for violation of terms of service.)", array(
        '@petition_id' => $signature['petition_id'],
      ));
      $user_message = t('This petition cannot accept more signatures. You can open a new petition on this issue here: @url.', array(
        '@url' => "{$base_url}/",
      ));
      $more_info = 'https://github.com/WhiteHouse/petitions/issues/74';
      $error_code = 74;
      return api_errors_throw_error($status_code, $developer_message, $user_message, $more_info, $error_code);

    case SIGNATURES_QUEUE_STATUS_NOT_FOUND:
      $developer_message = t("No such petition @petition_id", array(
        '@petition_id' => $signature['petition_id'],
      ));
      $user_message = t("The petition you are trying to sign doesn't exist.");
      $more_info = 'https://github.com/WhiteHouse/petitions/issues/76';
      $error_code = 76;
      return api_errors_throw_error($status_code, $developer_message, $user_message, $more_info, $error_code);

    case SIGNATURES_QUEUE_STATUS_SERVER_ERROR:
      $developer_message = t('Sorry. Service is not available right now.');
      $user_message = t('Sorry. We are unable to process your signature right now.');
      $more_info = 'https://github.com/WhiteHouse/petitions/issues/73';
      $error_code = 73;
      return api_errors_throw_error($status_code, $developer_message, $user_message, $more_info, $error_code);
  }
}
