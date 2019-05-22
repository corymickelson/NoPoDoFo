# API Documentation for Configure

- [API Documentation for Configure](#api-documentation-for-configure)
	- [NoPoDoFo Configure](#nopodofo-configure)
	- [Constructors](#constructors)
	- [Properties](#properties)
		- [enableDebugLogging](#enabledebuglogging)
	- [Methods](#methods)
		- [logFile](#logfile)
	
## NoPoDoFo Configure
Configure exposes runtime configuration options and loggers.

## Constructors

## Properties

### enableDebugLogging

Enabling debug logging. Debug logs are written to the root of the module as ```DbgLog.txt```.

## Methods

### logFile

```typescript
logFile(output: string): void
```

Set the loggers output location, this must be called prior to [enableDebugLogging](#enabledebuglogging).
