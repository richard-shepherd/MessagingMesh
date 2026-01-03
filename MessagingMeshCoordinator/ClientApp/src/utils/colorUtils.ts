/**
 * Hash-based color generation utilities
 * Provides deterministic, visually pleasing pastel colors for strings
 */

/**
 * FNV-1a hash function with bit-mixing for better color distribution.
 * This produces much more varied hues even for similar strings compared to djb2.
 * 
 * FNV-1a is a non-cryptographic hash function with good avalanche properties,
 * meaning small changes in input produce large changes in output.
 */
function hashString(str: string): number {
  // FNV-1a 32-bit constants
  const FNV_PRIME = 0x01000193;
  const FNV_OFFSET_BASIS = 0x811c9dc5;
  
  let hash = FNV_OFFSET_BASIS;
  
  // FNV-1a algorithm
  for (let i = 0; i < str.length; i++) {
    hash ^= str.charCodeAt(i);
    hash = Math.imul(hash, FNV_PRIME);
  }
  
  // Additional bit-mixing to improve distribution
  // This helps spread similar hash values across the color spectrum
  hash ^= hash >>> 16;
  hash = Math.imul(hash, 0x85ebca6b);
  hash ^= hash >>> 13;
  hash = Math.imul(hash, 0xc2b2ae35);
  hash ^= hash >>> 16;
  
  return Math.abs(hash);
}

/**
 * Generate a pastel color from a string using HSL color space
 * @param str - Input string to hash
 * @param saturation - Saturation percentage (default 65 for soft pastels)
 * @param lightness - Lightness percentage (default 90 for light colors)
 * @returns HSL color string
 */
export function generatePastelColor(
  str: string,
  saturation: number = 65,
  lightness: number = 90
): string {
  const hash = hashString(str);
  const hue = hash % 360;
  return `hsl(${hue}, ${saturation}%, ${lightness}%)`;
}

/**
 * Generate a gradient from a string with two related hues
 * @param str - Input string to hash
 * @returns CSS linear gradient string
 */
export function generatePastelGradient(str: string): string {
  const hash = hashString(str);
  const hue = hash % 360;
  const hue2 = (hue + 30) % 360; // Offset by 30 degrees for related color
  
  const saturation = 65;
  const lightness = 90;
  
  const color1 = `hsl(${hue}, ${saturation}%, ${lightness}%)`;
  const color2 = `hsl(${hue2}, ${saturation}%, ${lightness - 5}%)`;
  
  return `linear-gradient(135deg, ${color1} 0%, ${color2} 100%)`;
}
