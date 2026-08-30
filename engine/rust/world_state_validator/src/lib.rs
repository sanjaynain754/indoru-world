use serde::Deserialize;
use std::ffi::{c_char, CStr, CString};
use thiserror::Error;

#[derive(Debug, Deserialize)]
struct WorldDocument {
    world_id: String,
    name: String,
    country_count: usize,
    temporary_map_codes: bool,
    countries: Vec<CountryDocument>,
}

#[derive(Debug, Deserialize)]
struct CountryDocument {
    country_id: String,
    name: String,
    flag_id: String,
    status: String,
}

#[derive(Debug, Error)]
pub enum ValidationError {
    #[error("invalid JSON: {0}")]
    Json(#[from] serde_json::Error),
    #[error("world name must be Indoru")]
    WrongWorld,
    #[error("temporary map codes are not allowed")]
    TemporaryCodes,
    #[error("country count does not match registry")]
    CountryCount,
    #[error("country {0} has invalid name or flag")]
    CountryIdentity(String),
    #[error("country {0} has invalid status")]
    CountryStatus(String),
}

pub fn validate_world_json(input: &str) -> Result<(), ValidationError> {
    let world: WorldDocument = serde_json::from_str(input)?;
    if world.name != "Indoru" || world.world_id != "indoru-world-001" {
        return Err(ValidationError::WrongWorld);
    }
    if world.temporary_map_codes {
        return Err(ValidationError::TemporaryCodes);
    }
    if world.country_count != world.countries.len() {
        return Err(ValidationError::CountryCount);
    }
    for country in world.countries {
        if country.name.trim().is_empty() || !country.flag_id.starts_with("flag-") {
            return Err(ValidationError::CountryIdentity(country.country_id));
        }
        if country.status != "playable" && country.status != "coming_soon" {
            return Err(ValidationError::CountryStatus(country.country_id));
        }
    }
    Ok(())
}

/// Returns a newly allocated UTF-8 error string. Caller must release it with indoru_free_string.
#[no_mangle]
pub extern "C" fn indoru_validate_world_json(input: *const c_char) -> *mut c_char {
    if input.is_null() {
        return CString::new("null input").expect("static error has no NUL").into_raw();
    }
    let input = unsafe { CStr::from_ptr(input) };
    let result = std::str::from_utf8(input.to_bytes())
        .map_err(|error| error.to_string())
        .and_then(|text| validate_world_json(text).map_err(|error| error.to_string()));
    match result {
        Ok(()) => CString::new("").expect("empty string has no NUL").into_raw(),
        Err(error) => CString::new(error.replace('\0', " "))
            .expect("NUL removed from error")
            .into_raw(),
    }
}

#[no_mangle]
pub extern "C" fn indoru_free_string(value: *mut c_char) {
    if !value.is_null() {
        unsafe { drop(CString::from_raw(value)); }
    }
}

#[cfg(test)]
mod tests {
    use super::validate_world_json;

    #[test]
    fn accepts_valid_world_shape() {
        let json = r#"{"world_id":"indoru-world-001","name":"Indoru","country_count":1,"temporary_map_codes":false,"countries":[{"country_id":"country-indoru","name":"Indoru","flag_id":"flag-indoru","status":"playable"}]}"#;
        assert!(validate_world_json(json).is_ok());
    }

    #[test]
    fn rejects_temporary_codes() {
        let json = r#"{"world_id":"indoru-world-001","name":"Indoru","country_count":0,"temporary_map_codes":true,"countries":[]}"#;
        assert!(validate_world_json(json).is_err());
    }
}
