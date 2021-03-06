<?php

// $Id$

/**
 * @file
 *
 */
module_load_include('inc', 'fedora_repository', 'api/fedora_item');

/**
 * This class is used to get/set a fedora object's properties.
 */
class ObjectProperties {

  /**
   * The given objects PID.
   * 
   * @var string
   */
  protected $pid;
  /**
   * The object.
   * 
   * @var Fedora_Item
   */
  protected $item;
  /**
   * Object's Label.
   * 
   * @var string
   */
  protected $label;
  /**
   * Object's Owner.
   * 
   * @var string
   */
  protected $owner;
  /**
   * Object's State.
   * 
   * Either Active (A), Inactive (I) or Deleted (D)
   * 
   * @var string
   */
  protected $state;
  /**
   * The date that this object was created.
   * 
   * @var string
   */
  protected $created;
  /**
   * The date in which this object was last modified.
   * 
   * @var string
   */
  protected $modified;

  /**
   * Creates an object that fetchs the properties of the object identified by $PID. 
   * These properties can be changed, and committed back to fedora.
   * 
   * @param string $pid 
   */
  public function __construct($pid) {
    $this->pid = $pid;
    $this->item = new Fedora_Item($pid);
    $this->label = $this->getLabel();
    $this->owner = $this->getOwner();
    $this->state = $this->getState();
    $this->created = $this->getCreationDate();
    $this->modified = $this->getLastModifiedDate();
  }

  /**
   * Gets the object's label from fedora.
   *
   * @return string
   */
  private function getLabel() {
    return $this->item->objectProfile->objLabel;
  }

  /**
   * Gets the object's owner from fedora.
   *
   * @return string
   */
  private function getOwner() {
    $foxml = $this->item->export_as_foxml();
    $doc = new DOMDocument();
    $doc->loadXML($foxml);
    $xpath = new DOMXPath($doc);
    $result = $xpath->query("/*[local-name()='digitalObject']/*[local-name()='objectProperties']/*[local-name()='property'][@NAME='info:fedora/fedora-system:def/model#ownerId']/@VALUE");
    if ($result && $result->length == 1) {
      return $result->item(0)->value;
    }
    return '';
  }

  /**
   * Gets the object's state from fedora.
   *
   * @return string
   */
  private function getState() {
    $foxml = $this->item->export_as_foxml();
    $doc = new DOMDocument();
    $doc->loadXML($foxml);
    $xpath = new DOMXPath($doc);
    $result = $xpath->query("/*[local-name()='digitalObject']/*[local-name()='objectProperties']/*[local-name()='property'][@NAME='info:fedora/fedora-system:def/model#state']/@VALUE");
    if ($result && $result->length == 1) {
      $convert = array(
        'Active' => 'A',
        'Inactive' => 'I',
        'Deleted' => 'D',
      );
      $value = $result->item(0)->value;
      return isset($convert[$value]) ? $convert[$value] : $value;
    }
    return 'I'; // Default...
  }

  /**
   * Gets the object's creation date from fedora.
   *
   * @return string
   */
  private function getCreationDate() {
    return $this->item->objectProfile->objCreateDate;
  }

  /**
   * Gets the object's last modified date from fedora.
   *
   * @return string
   */
  private function getLastModifiedDate() {
    return $this->item->objectProfile->objLastModDate;
  }

  /**
   * Checks to see if the given object state is valid.
   * 
   * @param string $state 
   * 
   * @return boolean
   */
  private function validState($state) {
    return $state == "A" || $state == "I" || $state == "D";
  }

  /**
   * Propagates the given properties to this fedora object.
   * 
   * @param string $label
   * @param string $owner
   * @param string $state (Expected to be "A", "I", "D")
   */
  public function setProperties($label, $owner, $state) {
    if (!$this->validState($state)) {
      throw new Exception("'$state' is an invalid value for a fedora object's state.");
    }
    if ($this->item->modify_object($label, $state, $owner)) {
      $this->label = $label;
      $this->owner = $owner;
      $this->state = $state;
      return TRUE;
    }
    return FALSE;
  }

  /**
   * Gets the properties associated with this object as an array with the following properties.
   * 
   * label, owner, state, created, modified
   * 
   * @return array
   */
  public function getProperties() {
    return array(
      'label' => $this->label,
      'owner' => $this->owner,
      'state' => $this->state,
      'created' => $this->created,
      'modified' => $this->modified
    );
  }

}